using _Project._200_Dev.Utilities.Objects;

namespace _Project._200_Dev.Application_Management
{
    public class ApplicationManagerBehaviour : MonoSingleton<ApplicationManagerBehaviour>
    {
        public bool isQuitting { get; private set; }

        
        private void OnApplicationQuit()
        {
            isQuitting = true;
        }
    }
}