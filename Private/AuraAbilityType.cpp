#include "AuraAbilityType.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)   //定义某个结构体的网络序列化,Ar用于读取或写入序列化数据，Map用于解析UObject引用
{
	uint32 RepBits = 0;
	if(Ar.IsSaving())		//用于保存时进行序列化标记操作
	{
		if(bReplicateInstigator&&Instigator.IsValid())
		{
			RepBits |= 1<<0;		//将对应的标志位设为1，RepBit被初始化为0000 0000...(32位)，如果这个成员需要被序列化，则对应标志设置为1，变为....0000 0001
		}
		if(bReplicateEffectCauser&&EffectCauser.IsValid())
		{
			RepBits |=1<<1;
		}
		if(AbilityCDO.IsValid())
		{
			RepBits |= 1<<2;
		}
		if(bReplicateSourceObject&&SourceObject.IsValid())
		{
			RepBits |= 1<<3;
		}
		if(Actors.Num()>0)
		{
			RepBits |= 1<<4;
		}
		if(HitResult.IsValid())
		{
			RepBits |= 1<<5;
		}
		if(bHasWorldOrigin)
		{
			RepBits |= 1<<6;
		}
		//以下是自定义变量的序列化

		if(bIsBlocked)
		{
			RepBits |= 1<<7;
		}
		if(bIsCriticalHit)
		{
			RepBits |= 1<<8;
		}
		if (bIsSuccessfulDebuff)
		{
			RepBits |= 1 << 9;
		}
		if (DebuffDamage > 0.f)
		{
			RepBits |= 1 << 10;
		}
		if (DebuffDuration > 0.f)
		{
			RepBits |= 1 << 11;
		}
		if (DebuffFrequency > 0.f)
		{
			RepBits |= 1 << 12;
		}
		if (DamageType.IsValid())
		{
			RepBits |= 1 << 13;
		}
		if (!DeathImpulse.IsZero())
		{
			RepBits |= 1 << 14;
		}
		if (!KnockbackForce.IsZero())
		{
			RepBits |= 1 << 15;
		}
	}
	Ar.SerializeBits(&RepBits,15);		//序列化，根据上面计算到的值进行序列化,完成之后可以在加载时知道哪些成员是有效的,LengthBits表示有多少成员被序列化
	
	if(RepBits & (1<<0))			//检查哪些标志位是有效的，如果该标志位有效则对对应的成员进行序列化或反序列化
	{
		Ar<<Instigator;			//<<可以用作序列化也可以用作反序列化，取决于Ar是在保存还是加载,前者为序列化，反之为反序列化
	}
	if(RepBits & (1<<1))
	{
		Ar<<EffectCauser;
	}
	if(RepBits & (1<<2))
	{
		Ar<<AbilityCDO;
	}
	if(RepBits & (1<<3))
	{
		Ar<<SourceObject;
	}
	if(RepBits & (1<<4))
	{
		SafeNetSerializeTArray_Default<31>(Ar,Actors);
	}
	if(RepBits & (1<<5))
	{
		if(Ar.IsLoading())			//用于加载
		{
			if(!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar,Map,bOutSuccess);
	}
	if(RepBits & (1<<6))
	{
		Ar<<WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if(RepBits & (1<<7))
	{
		Ar<<bIsBlocked;
	}
	if(RepBits & (1<<8))
	{
		Ar<<bIsCriticalHit;
	}
	if (RepBits & (1 << 9))
	{
		Ar << bIsSuccessfulDebuff;
	}
	if (RepBits & (1 << 10))
	{
		Ar << DebuffDamage;
	}
	if (RepBits & (1 << 11))
	{
		Ar << DebuffDuration;
	}
	if (RepBits & (1 << 12))
	{
		Ar << DebuffFrequency;
	}
	if (RepBits & (1 << 13))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 14))
	{
		DeathImpulse.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 15))
	{
		KnockbackForce.NetSerialize(Ar, Map, bOutSuccess);
	}
	if(Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(),EffectCauser.Get());		//初始化发起者的能力组件
	}
	bOutSuccess = true;
	return true;
}
