using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace ivs_cs_demo
{
    /// <summary>
    /// Interaction logic for PlayOcxUserControl.xaml
    /// </summary>
    public partial class PlayOcxUserControl : UserControl
    {
        public PlayOcxViewModel playOcxViewModel { get; set; }
        public PlayOcxUserControl()
        {
            InitializeComponent();
            this.DataContext = playOcxViewModel;

            playOcxViewModel = new PlayOcxViewModel();
            this.dockPnlPlayer.Children.Add(playOcxViewModel.Player.OcxElement);         
        }
    }
}
