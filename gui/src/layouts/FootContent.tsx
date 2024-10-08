import React from 'react';
import { MessageOutlined, ProfileOutlined } from '@ant-design/icons';
import { Button, Modal } from 'antd';
import BackUpBuffer from '@/components/BackUpBuffer';

const FootContent = () => {
  const [modal, contextHolder] = Modal.useModal();
  return (
    <div className='bg-white flex flex-row-reverse p-2'>
      <MessageOutlined />
    </div>
  );
};

export default FootContent