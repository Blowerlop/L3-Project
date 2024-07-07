using UnityEngine;

namespace _Project._200_Dev.Menu
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
