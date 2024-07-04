namespace Project
{
    public interface ISilenceable
    {
        public bool IsSilenced { get; }
        
        public void Silence();
        public void Unsilence();
    }
}