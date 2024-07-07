using UnityEngine;

namespace Project
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
