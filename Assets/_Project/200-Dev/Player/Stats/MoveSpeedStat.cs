namespace Project
{
    public class MoveSpeedStat : Stat<int>
    {
        /// <summary>
        /// Slows the speed by the given amount
        /// </summary>
        /// <param name="amount"></param>
        /// <returns>
        /// How much the speed was slowed (not always the same as the amount, if clamped)
        /// </returns>
        public int Slow(int amount)
        
        {
            int lastMs = value;
            
            value -= amount;
            
            int newMs = value;
            
            return lastMs - newMs;
        }
    }
}
