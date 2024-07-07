using UnityEngine;

namespace _Project._200_Dev.Spells
{
    public class TrailRendererHandler : MonoBehaviour
    {
        void OnDestroy()
        {
            TrailRenderer myTrail = GetComponentInChildren<TrailRenderer>();
            if (myTrail != null)
            {
                myTrail.autodestruct = true;
                myTrail.transform.parent = null;
            }
        }
    }
}
