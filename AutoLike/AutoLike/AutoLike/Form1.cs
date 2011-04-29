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
            String url = urlText.Text;
            WebClient client = new WebClient();
            NameValueCollection myNameValueCollection = new NameValueCollection();
            myNameValueCollection.Add("charset_test", "&euro;,&acute;,€,’,水,?,?");
            myNameValueCollection.Add("lsd", "wVKoH");
            myNameValueCollection.Add("locale", "zh_TW");
            myNameValueCollection.Add("email", "fishbowformoon@gmail.com");
            myNameValueCollection.Add("pass", "761280362");
            url = "https://www.facebook.com/login.php?login_attempt=1";
            client.Headers.Add("user-agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
            //Stream data = client.OpenRead(url);
            //StreamReader reader = new StreamReader(data);
            //string s = reader.ReadToEnd();
            //resultText.Text = s;
            //data.Close();
            //reader.Close();
            byte[] responseArray = client.UploadValues(url, myNameValueCollection);
            resultText.Text = Encoding.UTF8.GetString(responseArray);
        }
    }
}
