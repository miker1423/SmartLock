using SmartLock.Mobile.ViewModels;
using System.ComponentModel;
using Xamarin.Forms;

namespace SmartLock.Mobile.Views
{
    public partial class ItemDetailPage : ContentPage
    {
        public ItemDetailPage()
        {
            InitializeComponent();
            BindingContext = new ItemDetailViewModel();
        }
    }
}