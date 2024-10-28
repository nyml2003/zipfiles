import { Button, Checkbox, Form, Input, message } from 'antd';
import React from 'react';
import { useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { v4 as uuidv4 } from 'uuid';
import useApi from '@/hooks/useApi';
import { ApiEnum } from '@/apis';
import { PostCommitRequest, PostCommitResponse } from '@/apis/PostCommit';
import { GetFileListRequest, GetFileListResponse } from '@/apis/GetFileList';
import { FileType } from '@/types';
type BackupFormProps = Partial<{
  message: string;
  storagePath: string;
  isEncrypt: boolean;
  author: string;
  key?: string;
}>;

const initialState: BackupFormProps = {
  isEncrypt: false,
  author: 'root',
};

interface File {
  name: string;
  type: FileType;
}

const BackupOption: React.FC = () => {
  const [form] = Form.useForm();
  const files = useSelector((state: RootState) => state.createCommit.selectedFile.files);
  const directories = useSelector(
    (state: RootState) => state.createCommit.selectedFile.directories,
  );
  const [messageApi, contextHolder] = message.useMessage();
  const api = useApi();

  // useEffect(() => {
  //   setFileData(files.map(file => file.path + '/' + file.name));
  // }, [files]);

  // useEffect(() => {
  //   directories.forEach(async path => {
  //     const files = await fetchAllFiles(path);
  //     setDirectoryData(prev => prev.concat(files));
  //   });
  // }, [directories]);

  const fetchFileList = async (path: string) => {
    const res = await api.request<GetFileListRequest, GetFileListResponse>(ApiEnum.GetFileList, {
      path,
    });
    return res.files;
  };

  /**
   * @brief 返回指定路径下的所有文件
   * @description 先获取指定路径下的一级文件，如果该文件是文件，直接加入集合，否则调用fetchAllFiles后，concat在后面
   * @param path
   * @returns String[] 所有文件的path+name形成的path的集合
   */
  const fetchAllFiles = async (path: string): Promise<string[]> => {
    const files = await fetchFileList(path);
    const allFiles: string[] = [];
    const promises: Promise<void>[] = [];

    files.forEach((file: File) => {
      if (file.type === FileType.Directory) {
        promises.push(
          fetchAllFiles(path + '/' + file.name).then(subFiles => {
            allFiles.push(...subFiles);
          }),
        );
      }
      allFiles.push(path + '/' + file.name);
    });

    await Promise.all(promises);
    return allFiles;
  };

  const onFinish = async (values: Required<BackupFormProps>) => {
    if ((!files || files.length === 0) && (!directories || directories.length === 0)) {
      messageApi.error('请选择文件');
      return;
    }
    const uuid = uuidv4();
    const fileData = files.map(file => file.path + '/' + file.name);
    let dirData: string[] = [];

    try {
      // 使用 Promise.all 处理所有的异步操作
      const dirPromises = directories.map(async path => {
        const files = await fetchAllFiles(path);
        return files;
      });
      const dirResults = await Promise.all(dirPromises);
      dirData = dirResults.flat(); // 将所有目录的结果合并到 dirData

      const backupFiles = [...fileData, ...dirData];
      const request: PostCommitRequest = {
        files: backupFiles,
        ...values,
        uuid,
        createTime: Date.now(),
      };
      await api.request<PostCommitRequest, PostCommitResponse>(ApiEnum.PostCommit, request);
      messageApi.success('备份成功');
    } catch (error) {
      messageApi.error('备份过程中发生错误');
    }
  };
  return (
    <div className='p-2'>
      备份文件设置
      {contextHolder}
      <div className='fade-in-down p-4 max-w-3xl'>
        <Form
          labelCol={{ span: 4 }}
          wrapperCol={{ span: 14 }}
          layout='horizontal'
          variant='outlined'
          scrollToFirstError
          form={form}
          initialValues={initialState}
          onFinish={onFinish}>
          <Form.Item
            label='是否加密'
            name='isEncrypt'
            valuePropName='checked'
            rules={[{ required: true, message: '请选择是否加密' }]}>
            <Checkbox />
          </Form.Item>
          <Form.Item shouldUpdate noStyle>
            {() => {
              const encrypt = form.getFieldValue('isEncrypt');
              return encrypt ? (
                <Form.Item
                  label='密码'
                  name='key'
                  rules={[
                    {
                      required: true,
                      message: '请输入密码',
                    },
                  ]}
                  shouldUpdate>
                  <Input />
                </Form.Item>
              ) : null;
            }}
          </Form.Item>
          <Form.Item
            label='commit'
            name='message'
            rules={[{ required: true, message: '请输入commit信息' }]}
            valuePropName='value'>
            <Input />
          </Form.Item>

          <Form.Item label='作者' name='author' rules={[{ required: true, message: '请输入作者' }]}>
            <Input />
          </Form.Item>
          <Form.Item wrapperCol={{ offset: 4, span: 14 }}>
            <Button type='primary' htmlType='submit'>
              提交
            </Button>
          </Form.Item>
        </Form>
      </div>
    </div>
  );
};

export default BackupOption;
