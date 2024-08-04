using System;
using System.Globalization;
using System.IO;
using System.Text;
using System.Threading;
using _Project._200_Dev.Console;
using Sirenix.OdinInspector;
using UnityEngine;

namespace _Project._200_Dev.Logs.LogFileExporter
{
    public static class CustomLogger
    {
        #region Variables
        [Title("Log Colors")] 
        // Log
        public static readonly Color logColor = new Color(1f, 1f, 1f, 1f);
        public static readonly string logColorHexadecimal = ColorUtility.ToHtmlStringRGB(logColor);
        // Warning
        public static readonly Color logWarningColor = new Color(1f, 0.996f, 0f, 1f);
        public static readonly string logWarningColorHexadecimal = ColorUtility.ToHtmlStringRGB(logWarningColor);
        // Error
        public static readonly Color logErrorColor = new Color(1f, 0.067f, 0f, 1f);
        public static readonly string logErrorColorHexadecimal = ColorUtility.ToHtmlStringRGB(logErrorColor);
        // Exception
        public static readonly Color logExceptionColor = new Color(92f, 39f, 81f, 1f);
        public static readonly string logExceptionColorHexadecimal = ColorUtility.ToHtmlStringRGB(logExceptionColor);
        // Assert
        public static readonly Color logAssertColor = new Color(138f, 155f, 104f, 1f);
        public static readonly string logAssertColorHexadecimal = ColorUtility.ToHtmlStringRGB(logAssertColor);
        
        // Log Saving
        private const string _LOG_SAVER_DEFAULT_FILE_PATH = "/_Project/Data/Log.txt";
        private static readonly string LogSaverFilePath = $"{Application.dataPath}{_LOG_SAVER_DEFAULT_FILE_PATH}";

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
            if (global::ParrelSync.ClonesManager.IsClone()) return;
            #endif
            
            StringBuilder stringBuilder = new StringBuilder();
            
            stringBuilder.AppendLine($"[{StripMilliseconds(DateTime.Now.TimeOfDay).ToString()}] {type.ToString()}");
            stringBuilder.AppendLine($"{condition} \n");
            stringBuilder.AppendLine($"{trace}");
            
            TxtFile.Write(LogSaverFilePath, stringBuilder.ToString());
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
            return logType switch
            {
                LogType.Log => logColor,
                LogType.Warning => logWarningColor,
                LogType.Error => logErrorColor,
                LogType.Assert => logAssertColor,
                LogType.Exception => logExceptionColor,
                _ => throw new ArgumentOutOfRangeException(nameof(logType), logType, null)
            };
        }
        
        public static string GetLogColorHexadecimal(LogType logType)
        {
            return logType switch
            {
                LogType.Log => logColorHexadecimal,
                LogType.Warning => logWarningColorHexadecimal,
                LogType.Error => logErrorColorHexadecimal,
                LogType.Assert => logAssertColorHexadecimal,
                LogType.Exception => logExceptionColorHexadecimal,
                _ => throw new ArgumentOutOfRangeException(nameof(logType), logType, null)
            };
        }
        #endregion
    }
}