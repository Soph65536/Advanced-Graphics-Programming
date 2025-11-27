#include "Mesh.h"

#include <d3d11.h>

#include "Renderer.h"
#include "ModelLoader.h"
#include "Debug.h"

Mesh::Mesh(Renderer& renderer, std::string objPath, bool doubleSided)
	: dev(renderer.GetDevice()), devCon(renderer.GetDeviceCon()), isDoubleSided(doubleSided) {
	
	ModelLoader ml{ objPath };

	//create the vertex buffer
	D3D11_BUFFER_DESC vbd = { 0 };
	vbd.Usage = D3D11_USAGE_DYNAMIC; //dynamic allows cpu write and gpu read
	vbd.ByteWidth = (unsigned int) ml.GetVertexBufferSize(); //size of buffer - size of vertex * num of vertices
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; //use as vertex buffer
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //allow CPU to write in buffer
	//create vertex buffer using the buffer data
	dev->CreateBuffer(&vbd, NULL, &vBuffer);
	if (vBuffer == 0) {
		LOG("Failed to create vertex buffer,,, apparently this wont fail");
		return;
	}

	//copy the vertices into the buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	devCon->Map(vBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); //map the buffer
	memcpy(ms.pData, ml.GetVertexData(), ml.GetVertexBufferSize()); //copy the data into the buffer
	devCon->Unmap(vBuffer, NULL);


	//create the index buffer
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = (int)ml.GetIndexBufferSize();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//define the resource data
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = ml.GetIndexData();

	//get the number of indices on the model
	indexCount = (int)ml.GetIndexCount();

	//create the index buffer with the device
	if (FAILED(dev->CreateBuffer(&ibd, &initData, &iBuffer))) {
		LOG("Failed to create index buffer");
		return;
	}
}

void Mesh::Render() {
	//select which primitive we are using
	devCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//select which vertex buffer to use
	UINT stride = sizeof(VertexPosUVNorm);
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	
	//select index buffer
	devCon->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	devCon->DrawIndexed(indexCount, 0, 0);
}