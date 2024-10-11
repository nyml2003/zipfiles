import React from 'react';
import { Button, Form, Input, InputNumber, Mentions, Select, Space } from 'antd'
import { FileType, Filter } from '@/types';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { updateFilter, updateIsFiltering } from '@/stores/file/reducer';
import { cleanObject } from '@/utils';

const fileTypeOptions = [
  { label: '文件', value: FileType.Regular },
  { label: '文件夹', value: FileType.Directory },
  { label: '链接', value: FileType.Symlink },
  { label: '块设备', value: FileType.Block },
  { label: '字符设备', value: FileType.Character },
  { label: 'FIFO', value: FileType.Fifo },
  { label: 'Socket', value: FileType.Socket },
];

const FilterForm: React.FC = () => {
  const [form] = Form.useForm();
  const filter = useSelector((state: RootState) => state.file.filter);
  const dispatch = useDispatch();

  const onReset = () => {
    form.resetFields();
    dispatch(updateFilter({}));
    dispatch(updateIsFiltering(false));
  };

  const onFinish = (values: Filter) => {
    console.log('Received values:', values);
    dispatch(updateFilter(values));
    dispatch(updateIsFiltering(false));
  };

  const handleSumbit = async () => {
    const values = await form.validateFields();
    const noNilValues = cleanObject(values);
    if (noNilValues.size) {
      const { min, max } = noNilValues.size;
      if (min !== undefined && max !== undefined && min > max) {
        form.setFields([
          {
            name: ['size', 'min'],
            errors: ['最小值不能大于最大值'],
          },
        ]);
        return;
      }
    }
    form.submit();
  };

  return (
    <div className='fade-in-down p-4 max-w-3xl'>
      <Form
        labelCol={{ span: 4 }}
        wrapperCol={{ span: 14 }}
        layout='horizontal'
        variant='outlined'
        scrollToFirstError
        form={form}
        initialValues={filter}
        onFinish={onFinish}>
        <Form.Item
          label='文件类型'
          name='type'
          rules={[{ required: false, message: 'Please select file type!' }]}>
          <Select options={fileTypeOptions} />
        </Form.Item>
        <Form.Item label='大小'>
          <Space.Compact>
            <Form.Item name={['size', 'min']} noStyle rules={[{ type: 'number' }]}>
              <InputNumber style={{ width: '40%' }} placeholder='最小值' min={0} />
            </Form.Item>
            <Input
              style={{
                width: '20%',
                textAlign: 'center',
              }}
              placeholder='~'
              disabled
            />
            <Form.Item name={['size', 'max']} noStyle rules={[{ type: 'number' }]}>
              <InputNumber style={{ width: '40%' }} placeholder='最大值' min={0} />
            </Form.Item>
          </Space.Compact>
        </Form.Item>

        <Form.Item
          label='所有者'
          name='owner'
          rules={[{ required: false, message: 'Please input owner!' }]}>
          <Mentions rows={1} placeholder='输入用户名' />
        </Form.Item>

        <Form.Item
          label='组'
          name='group'
          rules={[{ required: false, message: 'Please input group!' }]}>
          <Mentions rows={1} placeholder='输入组名' />
        </Form.Item>

        <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
          <Space>
            <Button type='primary' onClick={handleSumbit}>
              Submit
            </Button>
            <Button htmlType='button' onClick={onReset}>
              Reset
            </Button>
          </Space>
        </Form.Item>
      </Form>
    </div>
  );
};

export default FilterForm;
