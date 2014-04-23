using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using UPnPStack.CP;

namespace DeviceSpy
{
	/// <summary>
	/// Summary description for InvokeActionForm.
	/// </summary>
	public class InvokeActionForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button InvokeBtn;
		private System.Windows.Forms.Label ActionNameLabel;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.ColumnHeader directionHead;
		private System.Windows.Forms.ColumnHeader nameHead;
		private System.Windows.Forms.ColumnHeader valueHead;
		private System.Windows.Forms.ColumnHeader typeHead;
		private System.Windows.Forms.ListView m_ArgumentView;

		private Action m_Action;

		public InvokeActionForm(Action action)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			m_Action=action;

			ActionNameLabel.Text=m_Action.Name;

			
			foreach(Argument arg in m_Action.Arguments)
			{
				ListViewItem item=new ListViewItem(new String[]{"???",arg.Name,arg.DataType,arg.Direction==Argument.DirectionMode.IN?"in":"out"});

				if(arg.Direction==Argument.DirectionMode.OUT)
				{
					item.BackColor=Color.Yellow;
					item.Text="---";
				}

				m_ArgumentView.Items.Add(item);
			}			
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
			this.InvokeBtn = new System.Windows.Forms.Button();
			this.ActionNameLabel = new System.Windows.Forms.Label();
			this.m_ArgumentView = new System.Windows.Forms.ListView();
			this.valueHead = new System.Windows.Forms.ColumnHeader();
			this.nameHead = new System.Windows.Forms.ColumnHeader();
			this.typeHead = new System.Windows.Forms.ColumnHeader();
			this.directionHead = new System.Windows.Forms.ColumnHeader();
			this.SuspendLayout();
			// 
			// InvokeBtn
			// 
			this.InvokeBtn.Font = new System.Drawing.Font("Arial Black", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.InvokeBtn.Location = new System.Drawing.Point(368, 16);
			this.InvokeBtn.Name = "InvokeBtn";
			this.InvokeBtn.Size = new System.Drawing.Size(120, 64);
			this.InvokeBtn.TabIndex = 0;
			this.InvokeBtn.Text = "Invoke";
			this.InvokeBtn.Click += new System.EventHandler(this.OnInvoke);
			// 
			// ActionNameLabel
			// 
			this.ActionNameLabel.Font = new System.Drawing.Font("Arial Black", 24F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.ActionNameLabel.Location = new System.Drawing.Point(24, 24);
			this.ActionNameLabel.Name = "ActionNameLabel";
			this.ActionNameLabel.Size = new System.Drawing.Size(304, 48);
			this.ActionNameLabel.TabIndex = 1;
			this.ActionNameLabel.Text = "Action Name";
			// 
			// m_ArgumentView
			// 
			this.m_ArgumentView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
																							 this.valueHead,
																							 this.nameHead,
																							 this.typeHead,
																							 this.directionHead});
			this.m_ArgumentView.Font = new System.Drawing.Font("Arial Black", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.m_ArgumentView.ForeColor = System.Drawing.Color.DodgerBlue;
			this.m_ArgumentView.FullRowSelect = true;
			this.m_ArgumentView.GridLines = true;
			this.m_ArgumentView.LabelEdit = true;
			this.m_ArgumentView.Location = new System.Drawing.Point(16, 104);
			this.m_ArgumentView.MultiSelect = false;
			this.m_ArgumentView.Name = "m_ArgumentView";
			this.m_ArgumentView.Size = new System.Drawing.Size(472, 176);
			this.m_ArgumentView.TabIndex = 2;
			this.m_ArgumentView.View = System.Windows.Forms.View.Details;
			// 
			// valueHead
			// 
			this.valueHead.Text = "Value";
			this.valueHead.Width = 127;
			// 
			// nameHead
			// 
			this.nameHead.Text = "Name";
			this.nameHead.Width = 190;
			// 
			// typeHead
			// 
			this.typeHead.Text = "Type";
			this.typeHead.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.typeHead.Width = 100;
			// 
			// directionHead
			// 
			this.directionHead.Text = "Dir";
			this.directionHead.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			this.directionHead.Width = 40;
			// 
			// InvokeActionForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(504, 294);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.m_ArgumentView,
																		  this.ActionNameLabel,
																		  this.InvokeBtn});
			this.ForeColor = System.Drawing.SystemColors.WindowText;
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.Name = "InvokeActionForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
			this.Text = "Invoke Action";
			this.ResumeLayout(false);

		}
		#endregion

		private void OnInvoke(object sender, System.EventArgs e)
		{
			new System.Threading.Thread(new System.Threading.ThreadStart(this.InnerInvoke)).Start();
		}

		private void InnerInvoke()
		{
			Argument[] args=new Argument[m_Action.Arguments.Count];
			m_Action.Arguments.CopyTo(args);

			int i=0;
			foreach(Argument arg in m_Action.Arguments)
			{
				if(arg.Direction==Argument.DirectionMode.IN)
				{
					args[i].Name=arg.Name;
					args[i].Value=m_ArgumentView.Items[i].Text;
				}

				i++;
			}

			try
			{
				m_Action.Invoke(ref args);
			}
			catch(Exception err)
			{
				System.Windows.Forms.MessageBox.Show(this,err.Message,"Invoke Action error");
				return;
			}

			i=0;
			foreach(Argument arg in m_Action.Arguments)
			{
				if(arg.Direction==Argument.DirectionMode.OUT)
				{
					m_ArgumentView.Items[i].Text=args[i].Value;
				}

				i++;
			}
		}
	}
}
