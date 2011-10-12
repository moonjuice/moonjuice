using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.IO;

namespace EzQuickReply
{
    public partial class LoginForm : Form
    {
        public LoginForm()
        {
            InitializeComponent();
        }

        private void LoginButton_Click(object sender, EventArgs e)
        {
            ServicePointManager.Expect100Continue = false;
            string strId = "moonjuice";
            string strPassword = "teeth7612";
            string loginURL = "http://www.eyny.com/logging.php?action=login&amp;loginsubmit=yes";

            ASCIIEncoding encoding = new ASCIIEncoding();
            string postData;
            postData = ("formhash=5027383b&referer=index.php&loginfield=username&username=" + strId + "&password=" + strPassword + "&loginsubmit=true&cookietime=2592000");

            byte[] data = encoding.GetBytes(postData);

            CookieContainer cookieContainer = new CookieContainer();

            // Prepare web request...  
            HttpWebRequest myRequest = (HttpWebRequest)WebRequest.Create(loginURL);

            myRequest.CookieContainer = cookieContainer;

            myRequest.Method = "POST";
            myRequest.ContentType = "application/x-www-form-urlencoded";
            myRequest.ContentLength = data.Length;
            Stream newStream = myRequest.GetRequestStream();

            // Send the data.  
            newStream.Write(data, 0, data.Length);
            newStream.Close();

            // Get response  
            HttpWebResponse myResponse = (HttpWebResponse)myRequest.GetResponse();

            StreamReader reader = new StreamReader(myResponse.GetResponseStream(), Encoding.Default);
            string content = reader.ReadToEnd();
            if (content.Contains("歡迎您回來"))
            {
                //登入成功
                Global.cookieContainer = cookieContainer;
                this.Hide();
            }
            else
            {
                //登入失敗
                MessageBox.Show("登入失敗，請查看你的帳號密碼！", "錯誤!!",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Error);
            }
        }
    }
}
