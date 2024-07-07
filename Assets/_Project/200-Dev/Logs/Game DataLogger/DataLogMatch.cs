using System.Collections.Generic;

namespace _Project._200_Dev.Logs.Game_DataLogger
{
    [System.Serializable]
    public class DataLogTeamInfo{
        public string PCPlayerName;
        public string PCPlayerCharacter; 
        public string MobilePlayerName;
    }
    [System.Serializable]
    public class DataLogMatch
    {
        public int winnerTeam;
        public float time; 
        public List<DataLogTeamInfo> TeamInfoList = new List<DataLogTeamInfo>();

        public DataLogMatch(int teamIndex)
        {
            winnerTeam = teamIndex + 1 ; 
        }
    }
}
