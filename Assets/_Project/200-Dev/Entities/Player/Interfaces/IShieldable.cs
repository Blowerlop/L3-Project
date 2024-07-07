namespace _Project._200_Dev.Entities.Player.Interfaces
{
    public interface IShieldable
    {
        /// <summary>
        /// </summary>
        /// <param name="modifier"></param>
        /// <returns>id of this shield</returns>
        public int Shield(int modifier);
        public void UnShield(int shieldId);
    }
}