using Microsoft.EntityFrameworkCore;
using Proiect.Data;
using Proiect.Endpoints;


var builder = WebApplication.CreateBuilder(args);
builder.WebHost.ConfigureKestrel(options =>
{
    options.ListenAnyIP(5018); // Use your desired port here
});
// Add services
builder.Services.AddDbContext<DataContext>(options =>
    options.UseInMemoryDatabase("DataDb")); // For simplicity, using in-memory DB
builder.Services.AddCors(options =>
{
    options.AddDefaultPolicy(policy =>
    {
        policy.AllowAnyOrigin()
            .AllowAnyHeader()
            .AllowAnyMethod();
    });
});
var app = builder.Build();

app.UseWebSockets();

app.UseCors();
// Configure middleware

app.MapGet("/", () => "Hello World!");

// Map endpoints from DataEndPoints class
DataEndPoints.MapEndpoints(app);

app.Run();