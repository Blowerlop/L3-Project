using System;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;

namespace _Project._200_Dev.Utilities.Extensions
{
    public static class StringExtensions
    {
        public static string SeparateContent(this string text)
        {
            return string.Concat(text.Select(x => Char.IsUpper(x) ? " " + x : x.ToString())).TrimStart(' ');
        }
        
        public static string ExtractNumber(this string text)
        {
            var match = Regex.Match(text, @"([-+]?[0-9]*\.?[0-9]+)");
            if (match.Success)
                return (match.Groups[1].Value);

            return string.Empty;
        }
        
        public static string RemoveFirstLine(this string text)
        {
            return text.Substring(text.IndexOf("\n", StringComparison.Ordinal)+1);
        }
        
        public static string FollowCasePattern(this string text, string target)
        {
            int textLength = text.Length;
            if (textLength < target.Length) throw new ArgumentOutOfRangeException();
            
            StringBuilder stringBuilder = new StringBuilder();
            
            for (int i = 0; i < textLength; i++)
            {
                if (char.IsUpper(target[i]))
                {
                    stringBuilder.Append(char.ToUpper(text[i]));
                }
                else
                {
                    stringBuilder.Append(char.ToLower(text[i]));
                }
            }

            return stringBuilder.ToString();
        }
        
        public static string ConvertToValidIdentifier(this string input, bool isPath = false)
        {
            if (isPath) input = Path.GetFileNameWithoutExtension(input);
            
            // Replace all invalid characters
            input = Regex.Replace(input, "[^a-zA-Z0-9_]", "_", RegexOptions.Compiled);
            if (input.EndsWith('_')) input = input.Remove(input.Length - 1);
            
            return input;
        }
    }
}