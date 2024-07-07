namespace _Project._200_Dev.Utilities.Extensions
{
    public static class BooleanExtensions
    {
        public static int ToInt(this bool boolean) => boolean ? 1 : 0;

        public static bool ToBool(this int @int) => @int == 1 ? true : false;
    }
}