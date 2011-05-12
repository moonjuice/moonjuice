namespace AutoVote
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該處置 Managed 資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 設計工具產生的程式碼

        /// <summary>
        /// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改這個方法的內容。
        ///
        /// </summary>
        private void InitializeComponent()
        {
            this.mainPanel = new System.Windows.Forms.TableLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.nameText = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.phoneText = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.mailText = new System.Windows.Forms.TextBox();
            this.vote = new System.Windows.Forms.Button();
            this.result = new System.Windows.Forms.RichTextBox();
            this.mainPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainPanel
            // 
            this.mainPanel.ColumnCount = 3;
            this.mainPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 18.30986F));
            this.mainPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 59.85915F));
            this.mainPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 22.1831F));
            this.mainPanel.Controls.Add(this.label1, 0, 0);
            this.mainPanel.Controls.Add(this.nameText, 1, 0);
            this.mainPanel.Controls.Add(this.label2, 0, 1);
            this.mainPanel.Controls.Add(this.phoneText, 1, 1);
            this.mainPanel.Controls.Add(this.label3, 0, 2);
            this.mainPanel.Controls.Add(this.mailText, 1, 2);
            this.mainPanel.Controls.Add(this.vote, 2, 0);
            this.mainPanel.Controls.Add(this.result, 0, 3);
            this.mainPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainPanel.Location = new System.Drawing.Point(0, 0);
            this.mainPanel.Name = "mainPanel";
            this.mainPanel.RowCount = 4;
            this.mainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 11.45038F));
            this.mainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 10.68702F));
            this.mainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 11.45038F));
            this.mainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 67.17558F));
            this.mainPanel.Size = new System.Drawing.Size(284, 262);
            this.mainPanel.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 29);
            this.label1.TabIndex = 0;
            this.label1.Text = "姓名：";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // nameText
            // 
            this.nameText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.nameText.Location = new System.Drawing.Point(54, 3);
            this.nameText.Name = "nameText";
            this.nameText.Size = new System.Drawing.Size(163, 22);
            this.nameText.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label2.Location = new System.Drawing.Point(3, 29);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(45, 27);
            this.label2.TabIndex = 2;
            this.label2.Text = "手機：";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // phoneText
            // 
            this.phoneText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.phoneText.Location = new System.Drawing.Point(54, 32);
            this.phoneText.Name = "phoneText";
            this.phoneText.Size = new System.Drawing.Size(163, 22);
            this.phoneText.TabIndex = 3;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label3.Location = new System.Drawing.Point(3, 56);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(45, 29);
            this.label3.TabIndex = 4;
            this.label3.Text = "mail：";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // mailText
            // 
            this.mailText.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mailText.Location = new System.Drawing.Point(54, 59);
            this.mailText.Name = "mailText";
            this.mailText.Size = new System.Drawing.Size(163, 22);
            this.mailText.TabIndex = 5;
            // 
            // vote
            // 
            this.vote.Dock = System.Windows.Forms.DockStyle.Fill;
            this.vote.Location = new System.Drawing.Point(223, 3);
            this.vote.Name = "vote";
            this.mainPanel.SetRowSpan(this.vote, 3);
            this.vote.Size = new System.Drawing.Size(58, 79);
            this.vote.TabIndex = 6;
            this.vote.Text = "投票";
            this.vote.UseVisualStyleBackColor = true;
            this.vote.Click += new System.EventHandler(this.vote_Click);
            // 
            // result
            // 
            this.mainPanel.SetColumnSpan(this.result, 3);
            this.result.Dock = System.Windows.Forms.DockStyle.Fill;
            this.result.Location = new System.Drawing.Point(3, 88);
            this.result.Name = "result";
            this.result.Size = new System.Drawing.Size(278, 171);
            this.result.TabIndex = 7;
            this.result.Text = "";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.mainPanel);
            this.Name = "Form1";
            this.Text = "自動投票系統";
            this.mainPanel.ResumeLayout(false);
            this.mainPanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel mainPanel;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox nameText;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox phoneText;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox mailText;
        private System.Windows.Forms.Button vote;
        private System.Windows.Forms.RichTextBox result;
    }
}

