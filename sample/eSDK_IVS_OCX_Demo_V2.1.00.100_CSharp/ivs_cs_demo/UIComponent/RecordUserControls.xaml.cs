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
using System.Windows.Forms;

namespace ivs_cs_demo
{
    /// <summary>
    /// Interaction logic for RecordUserControls.xaml
    /// </summary>
    public partial class RecordUserControls : System.Windows.Controls.UserControl
    {
        public RecordViewModel recordViewModel { get; set; }
        public RecordUserControls()
        {
            InitializeComponent();
            recordViewModel = new RecordViewModel(this);
            this.DataContext = recordViewModel;
            this.CameraCodeTextBox.Text = "";
            this.PUCameraCodeTextBox.Text = "";
            this.PURecordTimeTextBox.Text = "";
            this.RecordTimeTextBox.Text = "";
            this.RecordParaTextBox.Text = @"<RecordParam>
  <RecordFormat>1</RecordFormat>
  <SplitterType>1</SplitterType>
  <SplitterValue>600</SplitterValue>
  <DiskWarningValue>1024</DiskWarningValue>
  <StopRecordValue>1024</StopRecordValue>
  <RecordTime>600</RecordTime>
  <NameRule>1</NameRule>
  <SavePath>D:\\</SavePath>
</RecordParam>";
        }

        private void LocalFileNameTextBox_GotMouseCapture(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    FolderBrowserDialog m_Dialog = new FolderBrowserDialog();
                    DialogResult result = m_Dialog.ShowDialog();

                    if (result == DialogResult.OK)
                    {
                        LocalFileNameTextBox.Text = m_Dialog.SelectedPath;
                    }
                    else if (result == DialogResult.Cancel)
                    {
                        return;
                    }
                    else
                    {
                        LocalFileNameTextBox.Text = string.Empty;
                    }
                }));
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
        }  
    }
}
