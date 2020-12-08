using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace SmartLock.Shared.Models
{
    public class TagRegistry
    {
        [DatabaseGenerated(DatabaseGeneratedOption.Identity)]
        [Key]
        public Guid Id { get; set; }
        public string Thumbprint { get; set; }
        public bool IsActive { get; set; }
        public Device OwnerDevice { get; set; }

    }
}
