namespace _Project._200_Dev.Entities.Player.Interfaces
{
    public interface ISilenceable
    {
        public bool IsSilenced { get; }
        
        public void Silence();
        public void Unsilence();
    }
}