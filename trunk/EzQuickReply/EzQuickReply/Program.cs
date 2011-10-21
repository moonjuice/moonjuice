using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.IO;
using HtmlAgilityPack;

namespace EzQuickReply
{
    static class Program
    {
        /// <summary>
        /// 應用程式的主要進入點。
        /// </summary>
        //[STAThread]
        static void Main()
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());
            //ServicePointManager.Expect100Continue = false;
            ////string strId = "moonjuice";
            ////string strPassword = "teeth7612";

            //ASCIIEncoding encoding = new ASCIIEncoding();
            ////string postData = "userid=" + strId;
            ////postData += ("&password=" + strPassword);
            //string postData;
            //postData =("formhash=5027383b&referer=index.php&loginfield=username&username=moonjuice&password=teeth7612&loginsubmit=true&cookietime=2592000");

            //byte[] data = encoding.GetBytes(postData);

            //CookieContainer cookieContainer = new CookieContainer();

            //// Prepare web request...  
            //HttpWebRequest myRequest =
            //(HttpWebRequest)WebRequest.Create("http://www.eyny.com/logging.php?action=login&amp;loginsubmit=yes");

            //myRequest.CookieContainer = cookieContainer;

            //myRequest.Method = "POST";
            //myRequest.ContentType = "application/x-www-form-urlencoded";
            //myRequest.ContentLength = data.Length;
            //Stream newStream = myRequest.GetRequestStream();

            //// Send the data.  
            //newStream.Write(data, 0, data.Length);
            //newStream.Close();

            //// Get response  
            //HttpWebResponse myResponse = (HttpWebResponse)myRequest.GetResponse();
            

            ////myRequest = (HttpWebRequest)WebRequest.Create("http://www.eyny.com/forum-27-1.html");
            ////myRequest.CookieContainer = cookieContainer;
            ////myResponse = (HttpWebResponse)myRequest.GetResponse();
            //StreamReader reader = new StreamReader(myResponse.GetResponseStream(), Encoding.Default);
            //string content = reader.ReadToEnd();
            ////用<th class="subject new">找出標題與網址
            //Console.WriteLine(content);

            //載入HTML
            HtmlDocument doc = new HtmlDocument();
            doc.Load("Test.htm");
            //找出所有class名稱為subject new的<th>的節點
            HtmlNodeCollection items = doc.DocumentNode.SelectNodes("//th[@class='subject new']");
            foreach (HtmlNode item in items)
            {
                // 裝載第一層查詢結果
                HtmlDocument linkContext = new HtmlDocument();
                linkContext.LoadHtml(item.InnerHtml);
                //找出所有<a>的節點，而且父節點名稱是span
                HtmlNode link = linkContext.DocumentNode.SelectSingleNode("//a[name(..)='span']");
                string title = link.InnerText;
                string url = link.GetAttributeValue("href", "not found");
                //找出所有<a>的節點，而且父節點名稱是em
                link = linkContext.DocumentNode.SelectSingleNode("//a[name(..)='em']");
                string type = link.InnerText;
            }
            
            Console.WriteLine("Test");
            Console.ReadLine();
        }
    }
}
