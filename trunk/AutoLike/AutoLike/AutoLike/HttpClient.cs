using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Configuration;
using System.Web;
using System.Net;


namespace AutoLike
{
    class HttpClient : WebClient
    {

        private CookieContainer cookieContainer;


        public HttpClient()
        {
            this.cookieContainer = new CookieContainer();
        }


        public HttpClient(CookieContainer cookies)
        {
            this.cookieContainer = cookies;
        }


        public CookieContainer Cookies
        {
            get { return this.cookieContainer; }
            set { this.cookieContainer = value; }
        }

        protected override WebRequest GetWebRequest(Uri address)
        {
            WebRequest request = base.GetWebRequest(address);
            if (request is HttpWebRequest)
            {
                HttpWebRequest httpRequest = request as HttpWebRequest;
                httpRequest.CookieContainer = cookieContainer;
            }
            return request;
        }
    }
}
