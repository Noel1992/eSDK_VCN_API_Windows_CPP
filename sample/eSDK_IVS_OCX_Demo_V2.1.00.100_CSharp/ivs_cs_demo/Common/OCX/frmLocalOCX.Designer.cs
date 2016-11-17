namespace ivs_cs_demo
{
    partial class frmLocalOCX
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmLocalOCX));
            this.axIVS_OCXPlayer = new AxIVS_OCXPlayer.AxIVS_OCXPlayer();
            ((System.ComponentModel.ISupportInitialize)(this.axIVS_OCXPlayer)).BeginInit();
            this.SuspendLayout();
            // 
            // axIVS_OCXPlayer
            // 
            this.axIVS_OCXPlayer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.axIVS_OCXPlayer.Enabled = true;
            this.axIVS_OCXPlayer.Location = new System.Drawing.Point(0, 0);
            this.axIVS_OCXPlayer.Name = "axIVS_OCXPlayer";
            this.axIVS_OCXPlayer.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axIVS_OCXPlayer.OcxState")));
            this.axIVS_OCXPlayer.Size = new System.Drawing.Size(292, 266);
            this.axIVS_OCXPlayer.TabIndex = 0;
            // 
            // frmLocalOCX
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(292, 266);
            this.Controls.Add(this.axIVS_OCXPlayer);
            this.Name = "frmLocalOCX";
            this.Text = "frmLocalOCX";
            ((System.ComponentModel.ISupportInitialize)(this.axIVS_OCXPlayer)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private AxIVS_OCXPlayer.AxIVS_OCXPlayer axIVS_OCXPlayer;


    }
}