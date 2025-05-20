using Microsoft.EntityFrameworkCore;

namespace Proiect.Data;

public class DataContext : DbContext
{
    public DataContext(DbContextOptions options) : base(options) {}
    
    public DbSet<Entities.Data> DataRecords { get; set; }
}