using System;

namespace UPnPStack
{
	/// <summary>
	/// Summary description for TypeConvert.
	/// </summary>
	public class TypeConvert
	{
		public static object StringToObject(Type t,string val)
		{
			if(t==typeof(bool))
				return int.Parse(val)>0?true:false;
			else if(t==typeof(int))
				return int.Parse(val);
			else if(t==typeof(uint))
				return uint.Parse(val);
			else if(t==typeof(short))
				return short.Parse(val);
			else if(t==typeof(ushort))
				return ushort.Parse(val);
			else if(t==typeof(float))
				return float.Parse(val);
			else if(t==typeof(string))
				return val;
			else
				return null;
		}

		public static string ObjectToString(object var)
		{
			if(var.GetType()==typeof(bool))
				return (bool)var?"1":"0";
			else
				return var.ToString();
		}
		
		public static string GetType(Type t)
		{
			if(t==typeof(bool))
				return "boolean";
			else if(t==typeof(int))
				return "i4";
			else if(t==typeof(uint))
				return "ui4";
			else if(t==typeof(short))
				return "i2";
			else if(t==typeof(ushort))
				return "ui2";
			else if(t==typeof(float))
				return "float";
			else if(t==typeof(string))
				return "string";
			else
				return "";
		}
	}
}
