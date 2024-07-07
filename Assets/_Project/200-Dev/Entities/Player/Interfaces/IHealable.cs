namespace _Project._200_Dev.Entities.Player.Interfaces
{
    public interface IHealable
    {
        public void Heal(int modifier);
        public void MaxHeal();
    }
}