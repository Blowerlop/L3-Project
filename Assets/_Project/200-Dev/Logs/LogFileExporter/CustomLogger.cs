using System;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading;
using Sirenix.OdinInspector;
using UnityEngine;

namespace Project
{
    public static class CustomLogger
    {
        #region Variables
        [Title("Log Colors")] 
        public static readonly Color logColor = new Color(1f, 1f, 1f, 1f);
        public static readonly Color logWarningColor = new Color(1f, 0.996f, 0f, 1f);
        public static readonly Color logErrorColor = new Color(1f, 0.067f, 0f, 1f);
        
        private const string _LOG_SAVER_DEFAULT_FILE_PATH = "/_Project/Data/Log.txt";
        private static readonly string LogSaverFilePath = $"{Application.dataPath}{_LOG_SAVER_DEFAULT_FILE_PATH}";

        private static readonly StringBuilder _stringBuilder = new StringBuilder();
        #endregion
        
        
        #region Methods
        
        [RuntimeInitializeOnLoadMethod(RuntimeInitializeLoadType.SubsystemRegistration)]
        private static void InitCustomLogger()
        {
            CreateLogFile();
            ClearLogFile();

            RegisterEvents();
        }

        private static void RegisterEvents()
        {
            Application.logMessageReceived += WriteLogToFile;
            Application.quitting += UnregisterEvents;
        }
        
        private static void UnregisterEvents()
        {
            Application.logMessageReceived -= WriteLogToFile;
            Application.quitting -= UnregisterEvents;
        }
        
        private static void CreateLogFile()
        {
            TxtFile.CreateFile(LogSaverFilePath);
        }

        private static void ClearLogFile()
        {
            TxtFile.Clear(LogSaverFilePath);
        }
         
        private static void WriteLogToFile(string condition, string trace, LogType type)
        {
            #if UNITY_EDITOR
            if (ParrelSync.ClonesManager.IsClone()) return;
            #endif
            
            _stringBuilder.AppendLine($"[{StripMilliseconds(DateTime.Now.TimeOfDay).ToString()}] {type.ToString()}");
            _stringBuilder.AppendLine($"{condition} \n");
            _stringBuilder.AppendLine($"{trace}");
            TxtFile.Write(LogSaverFilePath, _stringBuilder.ToString());
            
            _stringBuilder.Clear();
        }
        
        [ConsoleCommand("logs_export", "Send the current logs to the discord")]
        [Button]
        public static void ExportLogToDiscord()
        {
            if (File.Exists(LogSaverFilePath) == false)
            {
                Debug.LogError("There is no log file to send");
                return;
            }
            
            Thread thread = new Thread(SendFile);
            thread.Start();
            return;

            void SendFile()
            {
                Discord.SendFile(DateTime.Now.ToString(CultureInfo.GetCultureInfoByIetfLanguageTag("fr")), Discord.TxtFileFormat,
                    LogSaverFilePath, Discord.TxtFileFormat);
            }
        }
        
        private static TimeSpan StripMilliseconds(TimeSpan time)
        {
            return new TimeSpan(time.Days, time.Hours, time.Minutes, time.Seconds);
        }

        public static Color GetLogColor(LogType logType)
        {
            switch (logType)
            {
                case LogType.Log:
                    return logColor;

                case LogType.Warning:
                    return logWarningColor;
                
                case LogType.Error:
                case LogType.Assert:
                case LogType.Exception:
                    return logErrorColor;

                default:
                    throw new ArgumentOutOfRangeException(nameof(logType), logType, null);
            }
        }
        #endregion
    }
}