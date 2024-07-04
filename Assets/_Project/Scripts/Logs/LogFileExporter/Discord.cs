using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using UnityEngine;

namespace Project
{
    public sealed class Discord
    {
        private const string WebhookUrl = "";

        public const string TxtFileFormat = "text/plain";


        public static void SendFile(string message,
            // string filename,
            string fileFormat,
            string filepath,
            string application
        )
        {
            string fileName = Path.GetFileName(filepath);

            // Read file data
            using FileStream fs = new FileStream(filepath, FileMode.Open, FileAccess.Read);
            byte[] data = new byte[fs.Length];
            fs.Read(data, 0, data.Length);
            fs.Close();

            // Generate post objects
            Dictionary<string, object> postParameters = new Dictionary<string, object>
            {
                { "fileName", fileName },
                { "fileFormat", fileFormat },
                { "file", new FormUpload.FileParameter(data, fileName, application) },

                // postParameters.Add("username", defaultUserName);
                { "username", "LogBot" },
                { "content", message }
            };

            // Create request and receive response
            // HttpWebResponse webResponse = FormUpload.MultipartFormDataPost(webhookUrl, defaultUserAgent, postParameters);
            using HttpWebResponse webResponse =
                FormUpload.MultipartFormDataPost(WebhookUrl, "defaultUserAgent", postParameters);

            // Process response
            using Stream responseStream = webResponse.GetResponseStream();
            if (responseStream == null)
            {
                Debug.LogError("No WebResponse");
                return;
            }

            using StreamReader responseReader = new StreamReader(responseStream);
            string fullResponse = responseReader.ReadToEnd();
            webResponse.Close();
            Debug.Log($"Discord: file successfully sent \n {filepath}");

            //return string with response
        }

        private static class FormUpload //formats data as a multi part form to allow for file sharing
        {
            private static readonly Encoding _encoding = Encoding.UTF8;

            public static HttpWebResponse MultipartFormDataPost(string postUrl, string userAgent,
                Dictionary<string, object> postParameters)
            {
                string formDataBoundary = $"----------{Guid.NewGuid():N}";

                string contentType = "multipart/form-data; boundary=" + formDataBoundary;

                byte[] formData = GetMultipartFormData(postParameters, formDataBoundary);

                return PostForm(postUrl, userAgent, contentType, formData);
            }

            private static HttpWebResponse PostForm(string postUrl, string userAgent, string contentType,
                byte[] formData)
            {
                HttpWebRequest request = WebRequest.Create(postUrl) as HttpWebRequest;

                if (request == null)
                {
                    Debug.LogWarning("request is not a http request");
                    throw new NullReferenceException("request is not a http request");
                }

                // Set up the request properties.
                request.Method = "POST";
                request.ContentType = contentType;
                request.UserAgent = userAgent;
                request.CookieContainer = new CookieContainer();
                request.ContentLength = formData.Length;

                // Send the form data to the request.
                using (Stream requestStream = request.GetRequestStream())
                {
                    requestStream.Write(formData, 0, formData.Length);
                    requestStream.Close();
                }

                return request.GetResponse() as HttpWebResponse;
            }

            private static byte[] GetMultipartFormData(Dictionary<string, object> postParameters, string boundary)
            {
                Stream formDataStream = new MemoryStream();
                bool needsClrf = false;

                foreach (var param in postParameters)
                {
                    if (needsClrf)
                        formDataStream.Write(_encoding.GetBytes("\r\n"), 0, _encoding.GetByteCount("\r\n"));

                    needsClrf = true;

                    if (param.Value is FileParameter)
                    {
                        FileParameter fileToUpload = (FileParameter)param.Value;

                        string header =
                            $"--{boundary}\r\nContent-Disposition: form-data; name=\"{param.Key}\"; filename=\"{fileToUpload.fileName ?? param.Key}\"\r\nContent-Type: {fileToUpload.contentType ?? "application/octet-stream"}\r\n\r\n";

                        formDataStream.Write(_encoding.GetBytes(header), 0, _encoding.GetByteCount(header));

                        formDataStream.Write(fileToUpload.file, 0, fileToUpload.file.Length);
                    }
                    else
                    {
                        string postData =
                            $"--{boundary}\r\nContent-Disposition: form-data; name=\"{param.Key}\"\r\n\r\n{param.Value}";
                        formDataStream.Write(_encoding.GetBytes(postData), 0, _encoding.GetByteCount(postData));
                    }
                }

                // Add the end of the request.  Start with a newline
                string footer = "\r\n--" + boundary + "--\r\n";
                formDataStream.Write(_encoding.GetBytes(footer), 0, _encoding.GetByteCount(footer));

                // Dump the Stream into a byte[]
                formDataStream.Position = 0;
                byte[] formData = new byte[formDataStream.Length];
                formDataStream.Read(formData, 0, formData.Length);
                formDataStream.Close();

                return formData;
            }

            public class FileParameter
            {
                public byte[] file { get; set; }
                public string fileName { get; set; }
                public string contentType { get; set; }

                public FileParameter(byte[] file) : this(file, null)
                {
                }

                public FileParameter(byte[] file, string filename) : this(file, filename, null)
                {
                }

                public FileParameter(byte[] file, string filename, string contentType)
                {
                    this.file = file;
                    fileName = filename;
                    this.contentType = contentType;
                }
            }
        }
    }
}
