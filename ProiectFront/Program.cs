using ProiectFront.Clients;
using ProiectFront.Components;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddRazorComponents()
    .AddInteractiveServerComponents();
var proiectAppUrl = builder.Configuration["ProiectUrl"] ?? throw new Exception("ProiectUrl is not set");
builder.Services.AddHttpClient<DataClient>(client => client.BaseAddress = new Uri(proiectAppUrl));
builder.WebHost.ConfigureKestrel(serverOptions =>
{
    serverOptions.ListenAnyIP(5069); // HTTP port
    serverOptions.ListenAnyIP(5001, options => options.UseHttps()); // HTTPS port
});
var app = builder.Build();

// Configure the HTTP request pipeline.
if (!app.Environment.IsDevelopment())
{
    app.UseExceptionHandler("/Error", createScopeForErrors: true);
    // The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
    app.UseHsts();
}

app.UseHttpsRedirection();


app.UseAntiforgery();

app.MapStaticAssets();
app.MapRazorComponents<App>()
    .AddInteractiveServerRenderMode();

app.Run();