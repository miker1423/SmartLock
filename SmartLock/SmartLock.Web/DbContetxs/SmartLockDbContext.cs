using Microsoft.EntityFrameworkCore;
using SmartLock.Shared.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SmartLock.Web.DbContetxs
{
    public class SmartLockDbContext : DbContext
    {
        public DbSet<TagRegistry> TagRegistries { get; set; }
        public DbSet<Home> Homes { get; set; }
        public DbSet<Device> Devices { get; set; }
        public DbSet<User> Users { get; set; }

        public SmartLockDbContext(DbContextOptions options) : base(options) { }
    }
}
