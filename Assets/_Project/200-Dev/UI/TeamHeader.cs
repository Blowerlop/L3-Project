using System.Linq;
using UnityEngine;

namespace Project
{
    public class TeamHeader : MonoBehaviour
    {
        [SerializeField] private TeamHeaderItem prefab;
        [SerializeField] private Transform prefabParent;
        
        private void Start()
        {
            foreach(var user in UserInstanceManager.instance.GetUsersInstance())
            {
                var teamIndex = user.Team;

                var teamHeader = Instantiate(prefab, prefabParent);
                teamHeader.Init(user, teamIndex);
            }
        }
    }
}
