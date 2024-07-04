using UnityEngine;

namespace Project
{
    public static class PlayerData
    {
        public static string playerName
        {
            get => PlayerPrefs.GetString("PlayerName", "Unknown Name");
            set
            {
                PlayerPrefs.SetString("PlayerName", value);
                PlayerPrefs.Save();
            }
        }
    }
}
