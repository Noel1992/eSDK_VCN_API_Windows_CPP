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
    /// Interaction logic for DownloadAndCaptureUserControls.xaml
    /// </summary>
    public partial class DownloadAndCaptureUserControls : System.Windows.Controls.UserControl
    {
        public DownloadAndCaptureViewModel downloadAndCaptureViewModel { get; set; }
        public DownloadAndCaptureUserControls()
        {
            InitializeComponent();
            downloadAndCaptureViewModel = new DownloadAndCaptureViewModel(this);
            this.DataContext = downloadAndCaptureViewModel;

            List<string> list = new List<string>();
            list.Add("JPG");
            list.Add("BMP");
            this.PicType.ItemsSource = list;
            this.PicType.SelectedIndex = 0;

            this.StartTime.Text = System.DateTime.Today.ToString("yyyyMMddHHmmss");
            this.EndTime.Text = System.DateTime.Today.AddDays(1).ToString("yyyyMMddHHmmss");

            this.SaveFileName.Text = @"C:\\record";
            this.DownLoadPara.Text = @"<DownloadParam>
  <RecordFormat>1</RecordFormat>
  <SplitterType>1</SplitterType>
  <SplitterValue>100</SplitterValue>
  <DiskWarningValue>100</DiskWarningValue>
  <StopRecordValue>100</StopRecordValue>
  <NameRule>1</NameRule>
  <EncryptRecord>0</EncryptRecord>
  <SavePath>C:\\record</SavePath>
  <DownloadSpeed>1</DownloadSpeed>
</DownloadParam>";
            this.SnapshotFielName.Text = System.Windows.Forms.Application.StartupPath;
                
        }

        private void SaveFileName_GotMouseCapture(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() => 
                {
                    FolderBrowserDialog m_Dialog = new FolderBrowserDialog();
                    DialogResult result = m_Dialog.ShowDialog();

                    if (result == DialogResult.OK)
                    {
                        SaveFileName.Text = m_Dialog.SelectedPath;
                    }
                    else if (result == DialogResult.Cancel)
                    {
                        return;
                    }
                    else
                    {
                        SaveFileName.Text = string.Empty;
                    }
                }));            
            }
            catch (Exception ex)
            {
                LogService.Error(ex.ToString());
            }
        }

        private void SnapshotFielName_GotMouseCapture(object sender, System.Windows.Input.MouseEventArgs e)
        {
            try
            {
                System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    FolderBrowserDialog m_Dialog = new FolderBrowserDialog();
                    DialogResult result = m_Dialog.ShowDialog();

                    if (result == DialogResult.OK)
                    {
                        SnapshotFielName.Text = m_Dialog.SelectedPath;
                    }
                    else if (result == DialogResult.Cancel)
                    {
                        return;
                    }
                    else
                    {
                        SnapshotFielName.Text = string.Empty;
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
