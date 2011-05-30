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
using System.Collections.Specialized;

namespace AutoLike
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void enter_Click(object sender, EventArgs e)
        {

            var cookies = new CookieContainer();
            Cookie c = new Cookie("locale", "zh_TW");
            cookies.Add(new Uri("https://www.facebook.com/"), c);
            ServicePointManager.Expect100Continue = false;

            //var request = (HttpWebRequest)WebRequest.Create("https://www.facebook.com/");
            var request = (HttpWebRequest)WebRequest.Create("https://www.facebook.com/login.php?login_attempt=1");
            request.CookieContainer = cookies;
            request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)";
            request.Method = "POST";
            request.ContentType = "application/x-www-form-urlencoded";
            request.KeepAlive = true;
            using (var requestStream = request.GetRequestStream())
            using (var writer = new StreamWriter(requestStream))
            {
                writer.Write("charset_test=&euro;,&acute;,€,’,水,?,?&lsd=wVKoH&locale=zh_TW&email=fishbowformoon@gmail.com&pass=761280362");
            }

            using (var responseStream = request.GetResponse().GetResponseStream())
            using (var reader = new StreamReader(responseStream, Encoding.UTF8))
            {
                var result = reader.ReadToEnd();
                //resultText.DocumentText = (result);
            }

            request = (HttpWebRequest)WebRequest.Create("http://www.facebook.com/jerry751122");
            request.CookieContainer = cookies;
            request.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)";
            request.Method = "GET";
            request.ContentType = "application/x-www-form-urlencoded";
            request.KeepAlive = true;
            
            using (var responseStream = request.GetResponse().GetResponseStream())
            using (var reader = new StreamReader(responseStream, Encoding.UTF8))
            {
                var result = reader.ReadToEnd();
                resultText.Text = (result);
            }
        }
    }
}
