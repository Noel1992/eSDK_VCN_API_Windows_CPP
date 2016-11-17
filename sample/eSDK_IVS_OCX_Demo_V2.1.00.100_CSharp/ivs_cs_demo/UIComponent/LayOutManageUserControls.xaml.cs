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
    /// Interaction logic for LayOutManageUserControls.xaml
    /// </summary>
    public partial class LayOutManageUserControls : UserControl
    {
        public LayOutManageViewModel layOutManageViewModel { get; set; }
        public LayOutManageUserControls()
        {
            InitializeComponent();
            layOutManageViewModel = new LayOutManageViewModel(this);
            this.DataContext = layOutManageViewModel;

             List<string> list = new List<string>();
             list.Add(StringHelper.FindLanguageResource("SourceScale"));
             list.Add(StringHelper.FindLanguageResource("FullScale"));

            DisplayScale.ItemsSource = list;
            DisplayScale.SelectedIndex = 0;
            DisplayScale.ToolTip = StringHelper.FindLanguageResource("DisplayScaleToolTip");
        }
    }
}
