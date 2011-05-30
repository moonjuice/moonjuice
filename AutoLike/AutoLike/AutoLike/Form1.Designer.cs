namespace AutoLike
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
            this.enter = new System.Windows.Forms.Button();
            this.urlText = new System.Windows.Forms.TextBox();
            this.resultText = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // enter
            // 
            this.enter.Location = new System.Drawing.Point(326, 11);
            this.enter.Name = "enter";
            this.enter.Size = new System.Drawing.Size(29, 23);
            this.enter.TabIndex = 1;
            this.enter.Text = "Go";
            this.enter.UseVisualStyleBackColor = true;
            this.enter.Click += new System.EventHandler(this.enter_Click);
            // 
            // urlText
            // 
            this.urlText.Location = new System.Drawing.Point(13, 13);
            this.urlText.Name = "urlText";
            this.urlText.Size = new System.Drawing.Size(307, 22);
            this.urlText.TabIndex = 2;
            // 
            // resultText
            // 
            this.resultText.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.resultText.Location = new System.Drawing.Point(0, 41);
            this.resultText.Name = "resultText";
            this.resultText.Size = new System.Drawing.Size(367, 282);
            this.resultText.TabIndex = 3;
            this.resultText.Text = "";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(367, 323);
            this.Controls.Add(this.resultText);
            this.Controls.Add(this.urlText);
            this.Controls.Add(this.enter);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button enter;
        private System.Windows.Forms.TextBox urlText;
        private System.Windows.Forms.RichTextBox resultText;
    }
}

