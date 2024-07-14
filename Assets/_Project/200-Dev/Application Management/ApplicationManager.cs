namespace _Project._200_Dev.Application_Management
{
    public static class ApplicationManager
    {
        private static ApplicationManagerBehaviour instance => ApplicationManagerBehaviour.instance;
        
        
        public static bool isQuitting => ApplicationManagerBehaviour.IsQuitting;
        public static int mainThreadId => instance.mainThreadId;
        
        
        public static bool OnMainThread()
        {
            return instance.OnMainThread();
        }
    }
}