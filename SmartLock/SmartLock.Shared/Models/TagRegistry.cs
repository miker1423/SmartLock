using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmartLock.Shared.Models
{
    public class TagRegistry
    {
        public string Thumbprint { get; set; }
        public bool IsActive { get; set; }
        public Device OwnerDevice { get; set; }

    }
}
