using ProiectFront.Models;

namespace ProiectFront.Clients;

public class DataClient(HttpClient httpClient)
{
    public async Task<Data> GetDataAsync() => await httpClient.GetFromJsonAsync<Data>("api/data") ?? throw new Exception("Could not find data");
    // In your DataClient.cs file
    public async Task<string> TurnLedOnAsync()
    {
            var response = await httpClient.PostAsync("api/data/led/on", null);
            response.EnsureSuccessStatusCode();
            return await response.Content.ReadAsStringAsync();
    }

    public async Task<string> TurnLedOffAsync()
    {
        var response = await httpClient.PostAsync("api/data/led/off", null);
        response.EnsureSuccessStatusCode();
        return await response.Content.ReadAsStringAsync();
    }
    public async Task<List<Data>> GetFloodsAsync()
    {
        var floods = await httpClient.GetFromJsonAsync<List<Data>>("api/floods");
        return floods ?? new List<Data>();
    }
    public async Task DeleteFloodAsync(int id)
    {
        var response = await httpClient.DeleteAsync($"api/floods/{id}");
        response.EnsureSuccessStatusCode();
    }
}