using _Project._200_Dev.Utilities.Objects;

namespace _Project._200_Dev.Application_Management
{
    public class ApplicationManagerBehaviour : MonoSingleton<ApplicationManagerBehaviour>
    {
        [ClearOnReload] public static bool IsQuitting;
        public int mainThreadId { get; private set; }


        protected override void Awake()
        {
            base.Awake();
            
            mainThreadId = System.Threading.Thread.CurrentThread.ManagedThreadId;
        }

        private void OnApplicationQuit()
        {
            IsQuitting = true;
            mainThreadId = System.Threading.Thread.CurrentThread.ManagedThreadId;
        }

        public bool OnMainThread()
        {
            return System.Threading.Thread.CurrentThread.ManagedThreadId == mainThreadId;
        }
    }
}