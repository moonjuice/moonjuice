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
using System.Globalization;
using System.Threading;

namespace AutoVote
{
    /// <summary>
    /// 主要的介面類別
    /// </summary>
    public partial class Form1 : Form
    {
        private voteWork workerObject;
        private Thread workerThread;
        /// <summary>
        /// 建構子
        /// </summary>
        public Form1()
        {
            //初始化界面
            InitializeComponent();
            //新增執行緒任務
            workerObject = new voteWork(result, nameText.Text, phoneText.Text, mailText.Text);
            //新增執行緒
            workerThread = new Thread(workerObject.DoWork);
        }
        /// <summary>
        /// 執行續任務的類別
        /// </summary>
        class voteWork
        {
            private RichTextBox result;
            private String name, phone, mail;
            private volatile bool _shouldStop = false;
            /// <summary>
            /// 建構子
            /// </summary>
            /// <param name="r">RichTextBox，要顯示訊息的地方</param>
            /// <param name="n">姓名</param>
            /// <param name="p">電話</param>
            /// <param name="m">地址</param>
            public voteWork(RichTextBox r,String n,String p,String m)
            {
                this.result = r;
                this.name = n;
                this.mail = m;
                this.phone = p;
            }
            /// <summary>
            /// 執行續要執行的任務在這邊
            /// </summary>
            public void DoWork()
            {
                 while (!_shouldStop)
                 {
                     //如果投票成功，等60分鐘再投票
                    if (voteFunction())
                        waitMinute(60);
                    else
                        //如果失敗，則15分鐘在投票一次
                        waitMinute(15);
                 }
            }
            /// <summary>
            /// 執行緒停止要呼叫的function
            /// </summary>
            public void RequestStop()
            {
                _shouldStop = true;
#if DEBUG
                result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("_shouldStop : " + _shouldStop.ToString() + "\n"); }));
#endif
            }
            /// <summary>
            /// 真正投票的function
            /// </summary>
            private bool voteFunction()
            {
                System.Net.ServicePointManager.Expect100Continue = false;//解決417錯誤
                DateTime date = DateTime.Now;
                String dateString;
                String html, verifyCode;
                //取得時間
                dateString = date.ToString("yyyy-MM-dd HH:mm:ss", DateTimeFormatInfo.InvariantInfo);
                result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("取得時間 : " + dateString + "\n"); }));
                //取得本機IP，有可能為虛擬IP
                IPAddress myIP = new IPAddress(Dns.GetHostByName(Dns.GetHostName()).AddressList[0].Address);
                result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("取得IP : " + myIP.ToString() + "\n"); }));
                //取得驗證碼            
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create("http://events.fashionguide.com.tw/2011mucha/vote_process.php?seqno=28&photo_half=f63288d6c6f8d60696e3ca0203bbc11d.jpg&counter=87");
                request.UserAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.24 (KHTML, like Gecko) Chrome/11.0.696.65 Safari/534.24";
                request.Method = "GET";
                request.ContentType = "application/x-www-form-urlencoded";
                request.KeepAlive = true;
                using (var responseStream = request.GetResponse().GetResponseStream())
                using (var reader = new StreamReader(responseStream, Encoding.UTF8))
                {
                    html = reader.ReadToEnd();
                }
                verifyCode = html.Substring(html.IndexOf("==") + 3, 5);
                result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("取得驗證碼 : " + verifyCode + "\n"); }));
                //投票
                request.AllowAutoRedirect = false;
                request = (HttpWebRequest)WebRequest.Create("http://events.fashionguide.com.tw/2011mucha/vote_process.php?seqno=28&photo_half=f63288d6c6f8d60696e3ca0203bbc11d.jpg&counter=93");
                request.UserAgent = "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/534.24 (KHTML, like Gecko) Chrome/11.0.696.65 Safari/534.24";
                request.Method = "POST";
                request.KeepAlive = true;
                request.ContentType = "application/x-www-form-urlencoded";
                using (var requestStream = request.GetRequestStream())
                using (var writer = new StreamWriter(requestStream))
                {
                    writer.Write("voterName=" + name + "&candidate=28&IP=" + myIP.ToString() + "&voteTime=" + dateString + "&createDate=" + dateString + "&cellphone=" + phone + "&email=" + mail + "&newsCode=on&verifyCode=" + verifyCode + "&button=送出&MM_insert=formVote");
                }
                using (var responseStream = request.GetResponse().GetResponseStream())
                using (var reader = new StreamReader(responseStream, Encoding.UTF8))
                {
                    html = reader.ReadToEnd();
                }
                if (html.Contains("vote_fail"))
                {
                    result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("投票失敗!!\n"); }));
                    return false;
                }
                else
                {
                    result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("投票成功!!\n"); }));
                    return true;
                }
            }
            /// <summary>
            /// 讓執行緒等待一段時間
            /// </summary>
            /// <param name="m">等待的分鐘數</param>
            private void waitMinute(int m)
            {
                for (int i = 0; i < m; i++)
                {
                    if (this._shouldStop)
                        break;
                    else
                    {
                        result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("倒數" + (m - i) + "分鐘再投票一次!!\n"); }));
                        for (int j = 0; j < 60000; j++)
                        {
                            if (this._shouldStop)
                            {
                                result.BeginInvoke(new MethodInvoker(delegate { result.AppendText("已停止!!\n"); }));
                                break;
                            }
                            else
                                Thread.Sleep(1);
                        }
                    }
                }
            }
        }
        /// <summary>
        /// 按下投票按鈕的function
        /// </summary>
        /// <param name="sender">觸發的物件</param>
        /// <param name="e">觸發的事件</param>
        private void vote_Click(object sender, EventArgs e)
        {
#if DEBUG
            nameText.Text = "李友謙";
            mailText.Text = "jerry751122@gmail.com";
            phoneText.Text = "0930022177";
#endif
            // Create the thread object. This does not start the thread.
            if (vote.Text.Equals("投票"))
            {
                vote.Text = "停止";                
                // Start the worker thread.
                workerThread.IsBackground = true;
                workerThread.Start();
                while (!workerThread.IsAlive) ;
            } 
            else
            {
                vote.Text = "投票";
                if (workerThread.IsAlive)
                {
                    workerObject.RequestStop();
                    workerThread.Join();
                }
            }
        }
    }
}
