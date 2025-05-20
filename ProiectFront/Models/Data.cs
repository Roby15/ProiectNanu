using System.ComponentModel.DataAnnotations;

namespace ProiectFront.Models;


    public class Data
    {
        public int Id { get; set; }
        [Required]
        public double Temperature { get; set; }
        [Required]
        [Range(0, 100)]
        public double Humidity { get; set; }
    
        public bool Inundatie { get; set; }
    
        public bool LedState { get; set; }
        public DateTime datetime { get; set; }
    }
