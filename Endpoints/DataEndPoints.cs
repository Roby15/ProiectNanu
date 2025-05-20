using System.Collections.Concurrent;
using System.Net.WebSockets;
using System.Text;
using Microsoft.EntityFrameworkCore;
using Proiect.Data;
using System.Text.Json;
using System.Net.WebSockets;
using Proiect.Entities;
using Proiect.Data;


namespace Proiect.Endpoints;
public class DataEndPoints
{
    public static void MapEndpoints(WebApplication app)
    {
        ConcurrentBag<WebSocket> clients = new ConcurrentBag<WebSocket>();
        app.Map("/ws", async context =>
        {
            using var scope = app.Services.CreateScope();
            var dataContext = scope.ServiceProvider.GetRequiredService<DataContext>();
            if (!context.WebSockets.IsWebSocketRequest)
            {
                context.Response.StatusCode = 400;
                return;
            }
            using WebSocket webSocket = await context.WebSockets.AcceptWebSocketAsync();
            clients.Add(webSocket);
            Console.WriteLine("WebSocket connection established.");
            var buffer = new byte[1024 * 4];
            while (webSocket.State == WebSocketState.Open)
            {
                var result= await webSocket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
                if (result.MessageType == WebSocketMessageType.Close)
                {
                    await webSocket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Closing", CancellationToken.None);
                }
                else if(result.MessageType == WebSocketMessageType.Text)
                {
                    string message = System.Text.Encoding.UTF8.GetString(buffer, 0, result.Count);
                    Console.WriteLine($"Received message: {message}");
                    try
                    {
                        
                        
                        var options= new JsonSerializerOptions
                        {
                            PropertyNameCaseInsensitive = true
                        };
                        if (message.TrimStart().StartsWith("["))
                        {
                            // Handle array of Data objects
                            var externalDataList = JsonSerializer.Deserialize<List<Entities.Data>>(message, options);
                            if (externalDataList != null)
                            {
                                foreach (var externalData in externalDataList)
                                {
                                    // Add each record to the database
                                    var record = new Entities.Data
                                    {
                                        Id = externalData.Id,
                                        Temperature = externalData.Temperature,
                                        Humidity = externalData.Humidity,
                                        LedState = externalData.LedState,
                                        Inundatie = externalData.Inundatie,
                                        datetime = externalData.datetime
                                    };
                                    dataContext.DataRecords.Add(record);
                                    if (record.Inundatie)
                                    {
                                        byte[] alert = Encoding.UTF8.GetBytes("FLOOD_ALERT");
                                        foreach (var client in clients)
                                            if (client.State == WebSocketState.Open)
                                                await client.SendAsync(alert, WebSocketMessageType.Text, true, CancellationToken.None);
                                    }
                                }
                                await dataContext.SaveChangesAsync();
                                var oldFloods = await dataContext.DataRecords
                                    .Where(d => d.Inundatie)
                                    .OrderByDescending(d => d.datetime)
                                    .Skip(10)
                                    .ToListAsync();

                                if (oldFloods.Any())
                                {
                                    dataContext.DataRecords.RemoveRange(oldFloods);
                                    await dataContext.SaveChangesAsync();
                                }
                            }
                            
                        }
                        else
                        {
                            // Handle single Data object
                            var externalData = JsonSerializer.Deserialize<Entities.Data>(message, options);
                            if (externalData != null)
                            {
                                var record = new Entities.Data
                                {
                                    Id = externalData.Id,
                                    Temperature = externalData.Temperature,
                                    Humidity = externalData.Humidity,
                                    LedState = externalData.LedState,
                                    Inundatie = externalData.Inundatie,
                                    datetime = externalData.datetime
                                };
                                dataContext.DataRecords.Add(record);
                                await dataContext.SaveChangesAsync();
                                var oldFloods = await dataContext.DataRecords
                                    .Where(d => d.Inundatie)
                                    .OrderByDescending(d => d.datetime)
                                    .Skip(10)
                                    .ToListAsync();

                                if (oldFloods.Any())
                                {
                                    dataContext.DataRecords.RemoveRange(oldFloods);
                                    await dataContext.SaveChangesAsync();
                                }
                                if (record.Inundatie)
                                {
                                    byte[] alert = Encoding.UTF8.GetBytes("FLOOD_ALERT");
                                    foreach (var client in clients)
                                        if (client.State == WebSocketState.Open)
                                            await client.SendAsync(alert, WebSocketMessageType.Text, true, CancellationToken.None);
                                }
                                Console.WriteLine($"Temperature: {record.Temperature}, Humidity: {record.Humidity}, LED State: {record.LedState}, Inundatie: {record.Inundatie}");
                            }

                            

                            // Here you can process the data as needed
                            
                        }
                    }
                    catch (JsonException e)
                    {
                        Console.WriteLine(e.Message);
                        throw;
                    }
                }
            }
        });

        app.MapGet("/api/data", async (DataContext context) =>
        {
            try
            {   
                var data = await context.DataRecords.OrderByDescending(d => d.Id).FirstOrDefaultAsync();
                if (data != null)
                {
                    return Results.Ok(data);
                }

                return Results.NotFound();
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                throw;

            }
        });


        app.MapPost("/api/data/led/on", async (DataContext context) =>
        {
            // Get the last record
            var lastRecord = await context.DataRecords.OrderByDescending(d => d.Id).FirstOrDefaultAsync();
            if (lastRecord != null)
            {
                lastRecord.LedState = true;
                await context.SaveChangesAsync();
            }

            // Notify all WebSocket clients
            foreach (var client in clients)
                if (client.State == WebSocketState.Open)
                    await client.SendAsync(Encoding.UTF8.GetBytes("LED_ON"), WebSocketMessageType.Text, true, CancellationToken.None);

            
        });

        app.MapPost("/api/data/led/off", async(DataContext context)  =>
        {
            var lastRecord = await context.DataRecords.OrderByDescending(d => d.Id).FirstOrDefaultAsync();
            if (lastRecord != null)
            {
                lastRecord.LedState = false;
                await context.SaveChangesAsync();
            }
            foreach (var client in clients)
                if(client.State == WebSocketState.Open)
                    await client.SendAsync(Encoding.UTF8.GetBytes("LED_OFF"),WebSocketMessageType.Text,true,CancellationToken.None);
            return Results.Ok("LED OFF");
        });
        app.MapGet("/api/floods", async (DataContext context) =>
        {
            foreach (var client in clients)
                if (client.State == WebSocketState.Open)
                    await client.SendAsync(Encoding.UTF8.GetBytes("Inundatie"), WebSocketMessageType.Text, true, CancellationToken.None);
            try
            {
                // Get the latest 10 records with inundatie = true, ordered by datetime descending
                var floodRecords = await context.DataRecords
                    .Where(d => d.Inundatie)
                    .OrderByDescending(d => d.datetime)
                    .Take(10)
                    .ToListAsync();

                if (floodRecords.Any())
                {
                    return Results.Ok(floodRecords);
                }

                return Results.NotFound("No flood events recorded");
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                return Results.Problem($"Error retrieving flood data: {ex.Message}");
            }
        });
        app.MapDelete("/api/floods/{id:int}", async (int id, DataContext context) =>
        {
            var record = await context.DataRecords.FirstOrDefaultAsync(d => d.Id == id && d.Inundatie);
            if (record == null)
                return Results.NotFound("Flood record not found or not marked as inundatie.");
            foreach (var client in clients)
                if (client.State == WebSocketState.Open)
                    await client.SendAsync(Encoding.UTF8.GetBytes("DELETE_INUNDATIE:"+id), WebSocketMessageType.Text, true, CancellationToken.None);
            context.DataRecords.Remove(record);
            await context.SaveChangesAsync();
            return Results.Ok("Flood record deleted.");
        });
    }
}