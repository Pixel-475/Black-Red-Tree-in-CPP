#include<iostream>
#include<memory>//使用了智能指针shared_ptr,减少内存管理导致的错误
using namespace std;

/*红黑树的C++实现*/
/*红黑树的性质*/
//只有红黑节点
//根黑叶黑
//红点不连续
//任一节点到叶子经过的黑节点数量一致

//利用enum枚举了树节点的两种颜色
enum class Color { red, black };
//构建红黑树类，type是数据类型
template <typename type, typename vtype>
class BRTree {
private:
	/*构建了红黑树的节点结构体，nil节点，以及root节点*/
	struct BRTNode {
		type _data;
		vtype _value;
		Color _color;
		shared_ptr<BRTNode> _dad;
		shared_ptr<BRTNode> _lson;
		shared_ptr<BRTNode> _rson;
		//初始化赋值操作
		BRTNode(const type& key, const vtype& value, Color c, shared_ptr<BRTNode> d, shared_ptr<BRTNode> l, shared_ptr<BRTNode> r) :
			_data(key), _value(value), _color(c), _dad(d), _lson(l), _rson(r) {
		}
	};
	shared_ptr<BRTNode> nil;//叶节点，整棵树的叶子共用它
	shared_ptr<BRTNode> root;
public:
	/*红黑树的初始化操作*/
	BRTree()
	{
		nil = make_shared<BRTNode>(type(), vtype(), Color::black, nullptr, nullptr, nullptr);
		nil->_dad = nil;
		nil->_lson = nil;
		nil->_rson = nil;
		root = nil;
	}
private:

	//左旋，旋转后右子点y为根
	void lrotate(shared_ptr<BRTNode> x)
	{
		shared_ptr<BRTNode> y = x->_rson;
		x->_rson = y->_lson;//x分走y的左孩子
		if (y->_lson != nil)
			y->_lson->_dad = x;

		y->_dad = x->_dad;//y认父亲
		if (x->_dad == nil)//x为根
			root = y;
		else if (x == x->_dad->_lson)
			x->_dad->_lson = y;
		else
			x->_dad->_rson = y;

		y->_lson = x;
		x->_dad = y;
	}
	//右旋，旋转后左子点y为根
	void rrotate(shared_ptr<BRTNode> x) {
		shared_ptr<BRTNode> y = x->_lson;
		x->_lson = y->_rson;
		if (y->_rson != nil)
			y->_rson->_dad = x;

		y->_dad = x->_dad;
		if (x->_dad == nil)
			root = y;
		else if (x == x->_dad->_lson)
			x->_dad->_lson = y;
		else
			x->_dad->_rson = y;

		y->_rson = x;
		x->_dad = y;
	}
	// 查找相匹配的值，返回这个【节点】
	shared_ptr<BRTNode> search(type key)
	{
		shared_ptr<BRTNode> cur = root;
		while (cur != nil)
		{
			if (key == cur->_data)
				return cur;
			else if (key < cur->_data)
				cur = cur->_lson;
			else
				cur = cur->_rson;
		}
		return nil;
	}
	//找当前子树最小，用于寻找被删除节点的下一顺位
	shared_ptr<BRTNode> minimum(shared_ptr<BRTNode> node) {
		while (node->_lson != nil)
			node = node->_lson;
		return node;
	}
	void Afix(shared_ptr<BRTNode> A);//插入修复函数声明，在类外定义
public:
	void insert(type key, vtype value) //插入函数，插入key
	{
		shared_ptr<BRTNode> A = make_shared<BRTNode>(key, value, Color::red, nil, nil, nil);//插入节点
		shared_ptr<BRTNode> B = nil;//插入位置的父点
		shared_ptr<BRTNode> C = root;//插入指针，从root开始

		while (C != nil)//如果C不是nil,就往下递归
		{
			B = C;
			if (A->_data < C->_data)
				C = C->_lson;
			else
				C = C->_rson;
		}
		A->_dad = B;
		if (B == nil) root = A;//刚刚创建的情况
		else if (A->_data < B->_data) B->_lson = A;
		else B->_rson = A;

		Afix(A);
	}
private:
	//接驳函数，用于删除dadnode后接上他的子树sontree
	void transplant(shared_ptr<BRTNode>dadnode, shared_ptr<BRTNode>sontree)
	{
		//1,dadnode为根
		if (dadnode->_dad == nil) root = sontree;
		//2,dadnode为它父亲的左子树
		else if (dadnode == dadnode->_dad->_lson) dadnode->_dad->_lson = sontree;//修复严重问题：= ->==
		//3,dadnode为它父亲的右子树
		else dadnode->_dad->_rson = sontree;
		sontree->_dad = dadnode->_dad;
	}
	//删除修复声明
	void Dfix(shared_ptr<BRTNode>D);
public:
	//删除
	bool remove(type key)
	{
		shared_ptr<BRTNode> D = search(key);//D是要删除的目标节点
		if (D == nil) return 0;

		shared_ptr<BRTNode> PN = D;//实际被删的点
		shared_ptr<BRTNode> SN;//被删的点的替代节点（PN的子节点或nil)
		Color PN_COLOR = PN->_color;
		//D只有右子树，直接删D,用D的右孩子为替代节点SN,D此时是PN,所以PN没有用上
		if (D->_lson == nil)
		{
			SN = D->_rson;
			transplant(D, D->_rson);
		}
		//D只有左子树,与上面同理
		else if (D->_rson == nil)
		{
			SN = D->_lson;
			transplant(D, D->_lson);
		}
		else//D有双子树,最麻烦的情况，实际上删了D的后顺位PN，SN是PN的右子点或nil
		{
			PN = minimum(D->_rson);//PN:D的后顺位
			SN = PN->_rson;
			PN_COLOR = PN->_color; // 重新获取PN的颜色

			if (PN->_dad == D)//D后顺位PN是D的儿子
				SN->_dad = PN;
			else
			{
				//PN右子树（即SN)顶PN位置,PN没有左子树
				transplant(PN, PN->_rson);
				PN->_rson = D->_rson;
				PN->_rson->_dad = PN;
			}

			transplant(D, PN);//PN代替D
			PN->_lson = D->_lson;
			PN->_lson->_dad = PN;
			PN->_color = D->_color;
		}
		if (PN_COLOR == Color::black) Dfix(SN);//红色点删了不需要改变，删除黑点可能破坏“黑高一致”原则；从替代节点开始修复
		return 1;
	}
private:
	// 遍历
	void preorder(shared_ptr<BRTNode> R)
	{
		if (R == nil) return;
		cout << R->_data << "-" << R->_value << " ";
		preorder(R->_lson);
		preorder(R->_rson);
	}
	void inorder(shared_ptr<BRTNode> R)
	{
		if (R == nil) return;
		inorder(R->_lson);
		cout << R->_data << "-" << R->_value << " ";
		inorder(R->_rson);
	}
	void postorder(shared_ptr<BRTNode> R)
	{
		if (R == nil) return;
		postorder(R->_lson);
		postorder(R->_rson);
		cout << R->_data << "-" << R->_value << " ";
	}

public:
	/*遍历*/
	void Preorder_Traversal()
	{
		preorder(root);
		cout << endl;
	}
	void Inorder_Traversal()
	{
		inorder(root); cout << endl;
	}
	void Postorder_Traversal()
	{
		postorder(root); cout << endl;
	}
	/*寻找*/
	//判断
	int Find(type key)
	{
		shared_ptr<BRTNode> R = root;
		while (R != nil)
		{
			if (key < R->_data) R = R->_lson;
			if (key > R->_data) R = R->_rson;
			if (key == R->_data)
			{
				return 0;
			}
		}
		return 1;
	}
	/*求取*/
	//和BRTree::Find()差不多，但返回_value的值
	vtype Get_value(type key)
	{
		shared_ptr<BRTNode> R = root;
		while (R != nil)
		{
			if (key < R->_data) R = R->_lson;
			if (key > R->_data) R = R->_rson;
			if (key == R->_data)
			{
				return R->_value;
			}
		}
		return vtype();
	}
};

//插入修复函数,篇幅过大所以在类外定义
template <typename type, typename vtype>
void BRTree<type, vtype>::Afix(shared_ptr<BRTNode> A)
{
	while (A != root && A->_dad->_color == Color::red) //双红
	{
		if (A->_dad == A->_dad->_dad->_lson) //父为爷左孩子
		{
			shared_ptr<BRTNode> y = A->_dad->_dad->_rson;
			if (y->_color == Color::red) //叔节点为红，父辈和祖辈变色
			{
				A->_dad->_color = Color::black;
				y->_color = Color::black;
				A->_dad->_dad->_color = Color::red;
				A = A->_dad->_dad;//向上调整
			}
			else //叔为黑
			{
				if (A == A->_dad->_rson) //LR:变成LL型
				{
					A = A->_dad;
					lrotate(A);
				}
				//LL：爷变红，父变黑，右旋
				A->_dad->_color = Color::black;
				A->_dad->_dad->_color = Color::red;
				rrotate(A->_dad->_dad);
			}
		}
		else //父为右孩子，逻辑一致
		{
			shared_ptr<BRTNode> y = A->_dad->_dad->_lson;
			if (y->_color == Color::red)
			{
				A->_dad->_color = Color::black;
				y->_color = Color::black;
				A->_dad->_dad->_color = Color::red;
				A = A->_dad->_dad;
			}
			else
			{
				if (A == A->_dad->_lson)
				{
					A = A->_dad;
					rrotate(A);
				}
				A->_dad->_color = Color::black;
				A->_dad->_dad->_color = Color::red;
				lrotate(A->_dad->_dad);
			}
		}
	}
	root->_color = Color::black;
}
//删除修复函数，篇幅过大所以在类外定义
template <typename type, typename vtype>
void BRTree<type, vtype>::Dfix(shared_ptr<BRTNode>SN)
{
	while (SN != root && SN->_color == Color::black)//如果SN为红，直接变黑就可以补上这条路减少的黑高；如果SN为黑，那问题就复杂了
	{
		if (SN == SN->_dad->_lson)//SN是左孩子
		{
			shared_ptr<BRTNode> BN = SN->_dad->_rson;//其兄弟节点
			//1，兄弟为红，调整：兄变黑，父变红
			if (BN->_color == Color::red)
			{
				BN->_color = Color::black;
				SN->_dad->_color = Color::red;
				lrotate(SN->_dad);
				BN = SN->_dad->_rson;
			}
			//2.0，兄弟为黑，且双孩子为黑
			if (BN->_lson->_color == Color::black && BN->_rson->_color == Color::black)//nil的双子节点都指向nil,不会逻辑错误
			{
				BN->_color = Color::red;
				SN = SN->_dad;//变红，问题向上转移
			}
			//2.1，兄弟黑，左孩子红,右黑【2.1的目的是把LR或RL变成LL或RR型然后紧接2.2】
			else
			{
				if (BN->_rson->_color == Color::black)
				{
					BN->_lson->_color = Color::black;//孩子变黑
					BN->_color = Color::red;//BN变红
					rrotate(BN);
					BN = SN->_dad->_rson;
				}
				//2.2,兄弟黑，左黑右红
				BN->_color = SN->_dad->_color;
				SN->_dad->_color = Color::black;
				BN->_rson->_color = Color::black;
				lrotate(SN->_dad);
				SN = root;
			}
		}
		else //SN是右孩子
		{
			shared_ptr<BRTNode> BN = SN->_dad->_lson;//它的兄弟
			//兄弟为红
			if (BN->_color == Color::red) {
				BN->_color = Color::black;
				SN->_dad->_color = Color::red;
				rrotate(SN->_dad);
				BN = SN->_dad->_lson;
			}

			if (BN->_rson->_color == Color::black && BN->_lson->_color == Color::black)
			{
				BN->_color = Color::red;
				SN = SN->_dad;
			}
			else {
				if (BN->_lson->_color == Color::black) {
					BN->_rson->_color = Color::black;
					BN->_color = Color::red;
					lrotate(BN);
					BN = SN->_dad->_lson;
				}

				BN->_color = SN->_dad->_color;
				SN->_dad->_color = Color::black;
				BN->_lson->_color = Color::black;
				rrotate(SN->_dad);
				SN = root;
			}
		}
	}
	SN->_color = Color::black;
}


//应用测试：记忆数据，减少重复运算

//题目：
/*有N个士兵站成一队列, 现在需要选择几个士兵派去侦察。
为了选择合适的士兵, 多次进行如下操作: 如果队列超过三个士兵, 那么去除掉所有站立位置为奇数的士兵,
或者是去除掉所有站立位置为偶数的士兵。直到不超过三个战士，他们将被送去侦察。现要求统计按这样的方法，
总共可能有多少种不同的正好三个士兵去侦察的士兵组合方案。*/
BRTree<long long, long long> map;

int f(long long n)
{
	if (n <= 2)
		return 0;
	if (n == 3)
		return 1;
	if (map.Find(n) == 0)
		return map.Get_value(n);
	else
	{
		map.insert(n, f(n / 2) + f((n + 1) / 2));
		return map.Get_value(n);
	}
}

int main()
{
	long long n;
	cin >> n;
	while (n)
	{
		cout << f(n) << endl;
		cin >> n;
	}
}
