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
            /*
            String url = urlText.Text;
            WebClient client = new WebClient();
            NameValueCollection myNameValueCollection = new NameValueCollection();
            myNameValueCollection.Add("charset_test", "&euro;,&acute;,€,’,水,?,?");
            myNameValueCollection.Add("lsd", "wVKoH");
            myNameValueCollection.Add("locale", "zh_TW");
            myNameValueCollection.Add("email", "fishbowformoon@gmail.com");
            myNameValueCollection.Add("pass", "761280362");
            url = "https://www.facebook.com/login.php?login_attempt=1";
            //url = "https://www.facebook.com/";
            client.Headers.Add("user-agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
            //Stream data = client.OpenRead(url);
            //StreamReader reader = new StreamReader(data);
            //string s = reader.ReadToEnd();
            //resultText.Text = s;
            //data.Close();
            //reader.Close();
            byte[] responseArray = client.UploadValues(url, myNameValueCollection);
            resultText.Text = Encoding.UTF8.GetString(responseArray);
            */

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
            using (var requestStream = request.GetRequestStream())
            using (var writer = new StreamWriter(requestStream))
            {
                writer.Write("charset_test=&euro;,&acute;,€,’,水,?,?&lsd=wVKoH&locale=zh_TW&email=fishbowformoon@gmail.com&pass=761280362");
            }

            using (var responseStream = request.GetResponse().GetResponseStream())
            using (var reader = new StreamReader(responseStream, Encoding.UTF8))
            {
                var result = reader.ReadToEnd();
                //resultText.Text = (result);
            }
            resultText.Text = cookies.GetCookies(new Uri("http://www.facebook.com")).Count + "";
            foreach (Cookie cookie in cookies.GetCookies(new Uri("http://www.facebook.com")))
            {
                resultText.Text += cookie.Name + ":" + cookie.Value;
            }

//             CookieContainer cookieContainer = new CookieContainer();
//             NameValueCollection myNameValueCollection = new NameValueCollection();
//             myNameValueCollection.Add("charset_test", "&euro;,&acute;,€,’,水,?,?");
//             myNameValueCollection.Add("lsd", "wVKoH");
//             myNameValueCollection.Add("locale", "zh_TW");
//             myNameValueCollection.Add("email", "fishbowformoon@gmail.com");
//             myNameValueCollection.Add("pass", "761280362");
// 
// 
//             UTF8Encoding encoding = new UTF8Encoding();
//             string postData = "";
//             for (int i = 0; i < myNameValueCollection.Count; i++)
//                 postData += myNameValueCollection.GetKey(i) + "=" + myNameValueCollection.Get(i) + "&";
// 
//             byte[] data = encoding.GetBytes(postData);
// 
//             // Prepare web request...
//             HttpWebRequest myRequest =
//             (HttpWebRequest)WebRequest.Create("https://www.facebook.com/login.php?login_attempt=1");
// 
//             myRequest.CookieContainer = cookieContainer;
//             myRequest.UserAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)";
//             myRequest.Method = "POST";
//             myRequest.ContentType = "application/x-www-form-urlencoded";
//             myRequest.ContentLength = data.Length;
//             Stream newStream = myRequest.GetRequestStream();
// 
//             // Send the data.
//             newStream.Write(data, 0, data.Length);
//             newStream.Close();
// 
//             // Get response
//             HttpWebResponse myResponse = (HttpWebResponse)myRequest.GetResponse();
//             StreamReader reader = new StreamReader(myResponse.GetResponseStream(), Encoding.UTF8);
//             string content = reader.ReadToEnd();
//             resultText.Text = (content);
        }
    }
}
