using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Linq;
using System;
using Newtonsoft.Json;

namespace Project
{
    public static class DataLogger
    {

        public const string dataInternalDefaultStoragePath = "";
        public const string dataServerDefaultStoragePath = "";


        // Public fields
        public static float periodicUpdateTime = 0.05f;
        public static List<System.Object> Data { get; private set; } = new List<System.Object>();


        // Private fields
        private static string fileName;
        private static bool isLogFileCreated;
        private static StreamWriter outputStreamWriter;
        private static bool isLogging;


        // Event
        public delegate void OnDataLoggerEvent();
        public static event OnDataLoggerEvent OnLogCreated;
        public static event OnDataLoggerEvent OnLogClosed;


        public static string CurrentApplicationLogRootPath
        {
            get
            {
                return Directory.GetCurrentDirectory();
            }
        }

        public static string LogPath
        {
            get
            {
                return CurrentApplicationLogRootPath + "/Logs/";
            }
        }

        public static string FullPath
        {
            get
            {
                return LogPath + fileName;
            }
        }

        public static bool IsDataLoggerReady
        {
            get
            {
                return isLogging && isLogFileCreated;
            }
        }

        public static string ToJson()
        {
            if (Data.Count > 0)
            {
                try
                {
                    return JsonConvert.SerializeObject(Data, Formatting.Indented, new JsonSerializerSettings { ReferenceLoopHandling = ReferenceLoopHandling.Ignore });
                }
                catch (Exception e)
                {
                    throw new ArgumentException("Fail to convert data to json " + e.Message);
                }

            }
            return "";
        }


        public static void AddLogEntry(object logData)
        {
            if (IsDataLoggerReady)
            {
                Data.Add(logData);
            }

        }
        public static void CloseUserLogFile()
        {
            if (isLogFileCreated && isLogging)
            {
                isLogging = false;
                if (OnLogClosed != null)
                    OnLogClosed();
            }
        }
        public static bool CreateLogFile(string _fileName)
        {
            if (!Directory.Exists(LogPath))
                Directory.CreateDirectory(LogPath);

            fileName = _fileName;

            isLogFileCreated = true;

            if (isLogging == false)
            {
                isLogging = true;

                if (OnLogCreated != null)
                    OnLogCreated();
            }

            return true;
        }

        public static bool WriteToFile()
        {
            if (!isLogFileCreated)
                return false;

            try
            {
                string str = JsonConvert.SerializeObject(Data, Formatting.Indented, new JsonSerializerSettings { ReferenceLoopHandling = ReferenceLoopHandling.Ignore });

                using (var outputStreamWriter = new StreamWriter(FullPath))
                {
                    outputStreamWriter.Write(str);
                    outputStreamWriter.Close();

                }
            }
            catch (Exception e)
            {
                throw new ArgumentException("Fail to write file at path " + FullPath + ". " + e.Message);
            }


            return true;
        }

        public static void Flush()
        {
            if (!isLogging)
            {
                Data.Clear();
                fileName = "";
                isLogFileCreated = false;
            }


        }

        public static void LogTeamInfo(int teamIndex)
        {
            CreateLogFile(DateTime.Now.ToString("yyyyMMdd_HHmmss") + "_Log.json");

            if (UserInstanceManager.instance == null) return; 
            var user = UserInstanceManager.instance.GetUsersInstance();
            DataLogMatch logMatch = new DataLogMatch(teamIndex);
            logMatch.time = Time.timeSinceLevelLoad;
            for (int i = 0; i < 3; i++)
            {
                UserInstance PCUser = user.FirstOrDefault(x => x.Team == i);
                DataLogTeamInfo teamInfo = new DataLogTeamInfo();
                if (PCUser != null)
                {
                    teamInfo.PCPlayerCharacter = SOCharacter.GetCharacter(PCUser.CharacterId).characterName;
                    teamInfo.PCPlayerName = PCUser.PlayerName;
                    logMatch.TeamInfoList.Add(teamInfo);
                }
            }
            AddLogEntry(logMatch);
            WriteToFile();
            CloseUserLogFile();
            Flush();
        }
    }



}
