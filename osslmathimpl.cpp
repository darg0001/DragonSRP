
#include "osslmathimpl.hpp"
#include "osslhelp.hpp"

namespace Dsrp
{
	template<class HashFunctionPolicy> 
	OsslMathImpl<HashFunctionPolicy>::OsslMathImpl() :
		N(BN_new()),
		g(BN_new()),
		k(BN_new()),
		ctx(BN_CTX_new())
	{
		
	}
	
	template<class HashFunctionPolicy>
	OsslMathImpl<HashFunctionPolicy>::~OsslMathImpl()
	{
		BN_free(N);
		BN_free(g);
		BN_free(k);
		BN_CTX_free(ctx);
	}
	
	template<class HashFunctionPolicy> 
	bytes OsslMathImpl<HashFunctionPolicy>::setNg
	(Ng<HashFunctionPolicy> ng)
	{
		// checkNg()????
		// set that we set Ng as bool
		bytes2bignum(ng.getN(), N);
		bytes2bignum(ng.getg(), g);
		bytes2bignum(ng.getk(), k);
	}
	
	// A = g^a mod N
	template<class HashFunctionPolicy>
	bytes OsslMathImpl<HashFunctionPolicy>::calculateA
	(bytes aa)
	{
		BIGNUM *a = BN_new();
		BIGNUM *A = BN_new();
		
		bytes2bignum(aa, a);
		BN_mod_exp(A, g, a, N, ctx);
		
		bytes ret;
		bignum2bytes(A, ret);
		
		BN_free(a);
		BN_free(A);
		
		return ret;
	}
	
	// constraint safety check implementation !(A%N==0)
	template<class HashFunctionPolicy>
	bool OsslMathImpl<HashFunctionPolicy>::AisOK(bytes AA)
	{
		/* SRP-6a safety check */
		BIGNUM *A = BN_new();
		BIGNUM *tmp1 = BN_new();
		
		bytes2bignum(AA, A);
		
		BN_mod(tmp1, A, N, ctx);
		bool ret = !BN_is_zero(tmp1);
		BN_free(A);
		BN_free(tmp1);
		return ret;
	}
	
	// B = k*v + g^b
	template<class HashFunctionPolicy>
	bytes OsslMathImpl<HashFunctionPolicy>::calculateB
	(bytes verificator, bytes bb)
	{
		BIGNUM *b = BN_new();
		BIGNUM *B = BN_new();
		BIGNUM *v = BN_new();
		BIGNUM *tmp1 = BN_new();
		BIGNUM *tmp2 = BN_new();

		bytes2bignum(bb, b);
		bytes2bignum(verificator, v);
		
		// there is neccessary to add the SRP6a security check
		// /* SRP-6a safety check */
        //  BN_mod(tmp1, A, ng->N, ctx);
		
		BN_mod_mul(tmp1, k, v, N, ctx); // tmp1 = k * v
        BN_mod_exp(tmp2, g, b, N, ctx); // tmp2 = (g ^ b) % N
        BN_mod_add(B, tmp1, tmp2, N, ctx); // B = k * v + (g ^ b) % N
		
		bytes ret;
		bignum2bytes(B, ret);
		
		BN_free(b);
		BN_free(v);
		BN_free(tmp1);
		BN_free(tmp2);
		BN_free(B);
		
		return ret;
	}
	
	
	// u = HASH(A || B); where || is string concatenation
	template<class HashFunctionPolicy>
	bytes OsslMathImpl<HashFunctionPolicy>::calculateU
	(bytes AA, bytes BB)
	{
		HashFunctionPolicy hf;
		bytes aandb = AA;
		aandb.insert(aandb.end(), BB.begin(), BB.end());
		return hf.hash(aandb);
	}
	
	// S = (A *(v^u)) ^ b
	template<class HashFunctionPolicy>
	bytes OsslMathImpl<HashFunctionPolicy>::calculateSserver
	(bytes AA, bytes verificator, bytes uu, bytes bb)
	{
		BIGNUM *A = BN_new();
		BIGNUM *v = BN_new();
		BIGNUM *u = BN_new();
		BIGNUM *b = BN_new();
		BIGNUM *tmp1 = BN_new();
		BIGNUM *tmp2 = BN_new();
		BIGNUM *S = BN_new();
		
		bytes2bignum(AA, A);
		bytes2bignum(verificator, v);
		bytes2bignum(uu, u);
		bytes2bignum(bb, b);
		
		BN_mod_exp(tmp1, v, u, N, ctx);
        BN_mod_mul(tmp2, A, tmp1, N, ctx);
        BN_mod_exp(S, tmp2, b, N, ctx);
		
		bytes result;
		bignum2bytes(S, result);
		
		BN_free(A);
		BN_free(v);
		BN_free(u);
		BN_free(b);
		BN_free(tmp1);
		BN_free(tmp2);
		BN_free(S);
		
		return result;
	}
	
	template<class HashFunctionPolicy> 
	bytes OsslMathImpl<HashFunctionPolicy>::calculateSclient
	(bytes BB, bytes xx, bytes aa, bytes uu)
	{
		
	}
	
	template<class HashFunctionPolicy>
	bytes OsslMathImpl<HashFunctionPolicy>::generateRandom
	(unsigned int bits)
	{
		
	}
}
