using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace DeviceSpy
{
	/// <summary>
	/// Summary description for DeviceRootUrlForm.
	/// </summary>
	public class DeviceRootUrlForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button OKBtn;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox RootURL;

		public string m_RootURL;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public DeviceRootUrlForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.OKBtn = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.RootURL = new System.Windows.Forms.TextBox();
			this.groupBox1.SuspendLayout();
			this.SuspendLayout();
			// 
			// OKBtn
			// 
			this.OKBtn.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.OKBtn.Location = new System.Drawing.Point(352, 72);
			this.OKBtn.Name = "OKBtn";
			this.OKBtn.Size = new System.Drawing.Size(80, 23);
			this.OKBtn.TabIndex = 2;
			this.OKBtn.Text = "OK";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.AddRange(new System.Windows.Forms.Control[] {
																					this.RootURL});
			this.groupBox1.Location = new System.Drawing.Point(16, 8);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(416, 56);
			this.groupBox1.TabIndex = 3;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Network Url";
			// 
			// RootURL
			// 
			this.RootURL.Location = new System.Drawing.Point(8, 24);
			this.RootURL.Name = "RootURL";
			this.RootURL.Size = new System.Drawing.Size(400, 21);
			this.RootURL.TabIndex = 0;
			this.RootURL.Text = "http://";
			// 
			// DeviceRootUrlForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(448, 102);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.groupBox1,
																		  this.OKBtn});
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "DeviceRootUrlForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Device Spy";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.DeviceRootUrlForm_Closing);
			this.groupBox1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void DeviceRootUrlForm_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			m_RootURL=RootURL.Text;
		}

	}
}
