using UnityEngine;

namespace Project
{
    public class Billboard : MonoBehaviour
    {
        private Camera _cam;
        
        private void LateUpdate()
        {
            if (!_cam)
            {
                _cam = Camera.main;
                if(!_cam) return;
            }
            
            transform.rotation = Quaternion.LookRotation(-_cam.transform.forward);
        }
    }
}
