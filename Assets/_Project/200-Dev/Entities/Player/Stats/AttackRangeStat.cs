namespace _Project._200_Dev.Entities.Player.Stats
{
    public class AttackRangeStat : Stat<float>
    {
        public float AddRange(float amount)
        {
            float lastRange = value;
            
            value += amount;
            
            float newRange = value;
            
            return newRange - lastRange;
        }
    }
} 